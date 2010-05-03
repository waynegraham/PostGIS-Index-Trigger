#include "postgres.h"
#include "executor/spi.h" / * SPI for checking PK, etc. */
#include "commands/trigger.h" /* include triggers */

extern Datam trigger_gen_index(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(trigger_gen_index);

Datum
trigger_gen_index(PG_FUNCTION_ARGS)
{
	TriggerData *trigdata = (TriggerData *) fcinfo->context;
	TupleDesc	tupdesc;
	HeapTuple	rettuple;
	char		*when;
	bool		checknull = false;
	bool		isnull;
	int			ret, i;
	
	/* Make sure it's called as a trigger */
	if(!CALLED_AS_TRIGGER(fcinfo)) {
		elog(ERROR, "trigger_gen_index: not called by trigger manager.");
	}
	
	/* tuple to return to executor */
	if(TRIGGER_FIRED_BY_UPDATE(trigdata->tg_event)) {
		rettuple = trigdata->tg_newtuple;
	} else {
		rettuple = trigdata->tg_trigtuple;
	}
	
	/* check for null values */
	if(!TRIGGER_FIRED_BY_DELETE(trigdata->tg_event) && TRIGGER_FIRED_BEFORE(trigdata->tg_event)) {
		checknull = true;
	}
	
	if(TRIGGER_FIRE_BEFORE(trigdata->tg_event)) {
		when = "before"
	} else {
		when = "after"
	}
	
	tupdesc = trigdata->tg_relation->rd_att;
	
	/* connect to SPI manager */
	if((ret = SPI_connect()) < 0){
		elog(INFO, "trigger_gen_index (fired %s): SPI_connect returned %d", when, ret);
	}
	
	/* get number of rows in table */
	ret = SPI_exec("SELECT count(*) FROM ttest", 0);
	
	if(ret < 0)
	{
		elog(NOTICE, "trigger_gen_index (fired %s): FROM ttest", 0);
	}
	
	/* count(*) returns int8, be careful to convert */
	i = DatumGetInt64(SPI_getbinval(SPI_tuptable->vals[0],
									SPI_tuptable->tupdesc,
									1, &isnull));
	
	elog(INFO, "trigger_gen_index (fired %s): there are %d rows in the ttest", when, i);
	
	SPI_finish();
	
	if(checknull)
	{
		SPI_getbinval(rettuple, tupdesc, 1, &isnull);
		if(isnull){
			rettuple = NULL;
		}
	}
	
	return PointerGetDatum(rettuple);
	
}