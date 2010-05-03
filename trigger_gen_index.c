#include "postgres.h"
#include "executor/spi.h" / * SPI for checking PK, etc. */
#include "commands/trigger.h" /* include triggers */

extern Datam trigger_gen_index(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(trigger_gen_index);

Datum
trigger_gen_index(PG_FUNCTION_ARGS)
{
	TriggerData *trigdata = (TriggerData *) fcinfo->context;
	
	
}