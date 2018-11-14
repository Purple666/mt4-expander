#include "expander.h"
#include "lib/conversion.h"
#include "lib/format.h"
#include "lib/math.h"
#include "lib/string.h"
#include "struct/xtrade/Test.h"


/**
 * Set the id of a TEST.
 *
 * @param  TEST* test
 * @param  int   id - test id (positive)
 *
 * @return int - the same id
 */
int WINAPI test_SetId(TEST* test, int id) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (id <= 0)                        return(error(ERR_INVALID_PARAMETER, "invalid parameter id: %d (not positive)", id));

   test->id = id;
   return(id);
}


/**
 * Set the creation time of a TEST.
 *
 * @param  TEST*    test
 * @param  datetime time - Unix timestamp (GMT)
 *
 * @return datetime - the same time
 */
datetime WINAPI test_SetCreated(TEST* test, datetime time) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (time <= 0)                      return(error(ERR_INVALID_PARAMETER, "invalid parameter time: %d (not positive)", time));

   test->created = time;
   return(time);
}


/**
 * Set the name of the tested strategy of a TEST.
 *
 * @param  TEST* test
 * @param  char* name
 *
 * @return char* - the same name
 */
const char* WINAPI test_SetStrategy(TEST* test, const char* name) {
   if ((uint)test < MIN_VALID_POINTER)         return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if ((uint)name < MIN_VALID_POINTER)         return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter name: 0x%p (not a valid pointer)", name));
   int len = strlen(name);
   if (!len || len > sizeof(test->strategy)-1) return((char*)error(ERR_INVALID_PARAMETER, "illegal length of parameter name \"%s\" (must be 1 to %d characters)", name, sizeof(test->strategy)-1));

   if (!strcpy(test->strategy, name))
      return(NULL);
   return(name);
}


/**
 * Set the reporting id of a TEST. Used for composition of TEST.reportingSymbol.
 *
 * @param  TEST* test
 * @param  int   id
 *
 * @return int - the same id
 */
int WINAPI test_SetReportId(TEST* test, int id) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (id < 0)                         return(error(ERR_INVALID_PARAMETER, "invalid parameter id: %d (not positive)", id));

   test->reportId = id;
   return(id);
}


/**
 * Set the reporting symbol of a TEST. Used for report charts.
 *
 * @param  TEST* test
 * @param  char* symbol
 *
 * @return char* - the same symbol
 */
const char* WINAPI test_SetReportSymbol(TEST* test, const char* symbol) {
   if ((uint)test   < MIN_VALID_POINTER) return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if ((uint)symbol < MIN_VALID_POINTER) return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter symbol: 0x%p (not a valid pointer)", symbol));
   int len = strlen(symbol);
   if (len > sizeof(test->reportSymbol)) return((char*)error(ERR_INVALID_PARAMETER, "illegal length of parameter symbol \"%s\" (max %d characters)", symbol, sizeof(test->reportSymbol)-1));

   if (!strcpy(test->reportSymbol, symbol))
      return(NULL);
   return(symbol);
}


/**
 * Set the symbol a TEST was run on.
 *
 * @param  TEST* test
 * @param  char* symbol
 *
 * @return char* - the same symbol
 */
const char* WINAPI test_SetSymbol(TEST* test, const char* symbol) {
   if ((uint)test   < MIN_VALID_POINTER)   return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if ((uint)symbol < MIN_VALID_POINTER)   return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter symbol: 0x%p (not a valid pointer)", symbol));
   int len = strlen(symbol);
   if (!len || len > sizeof(test->symbol)) return((char*)error(ERR_INVALID_PARAMETER, "illegal length of parameter symbol \"%s\" (must be 1 to %d characters)", symbol, sizeof(test->symbol)-1));

   if (!strcpy(test->symbol, symbol))
      return(NULL);
   return(symbol);
}


/**
 * Set the timeframe a TEST was run at.
 *
 * @param  TEST* test
 * @param  uint  timeframe
 *
 * @return uint - the same timeframe
 */
uint WINAPI test_SetTimeframe(TEST* test, uint timeframe) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if ((int)timeframe <= 0)            return(error(ERR_INVALID_PARAMETER, "invalid parameter timeframe: %d (not positive)", timeframe));

   test->timeframe = timeframe;
   return(timeframe);
}


/**
 * Set the time of a TEST's first tick.
 *
 * @param  TEST*    test
 * @param  datetime time - Unix timestamp (server time)
 *
 * @return datetime - the same time
 */
datetime WINAPI test_SetStartTime(TEST* test, datetime time) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (time <= 0)                      return(error(ERR_INVALID_PARAMETER, "invalid parameter time: %d (not positive)", time));

   test->startTime = time;
   return(time);
}


/**
 * Set the time of a TEST's last tick.
 *
 * @param  TEST*    test
 * @param  datetime time - Unix timestamp (server time)
 *
 * @return datetime - the same time
 */
datetime WINAPI test_SetEndTime(TEST* test, datetime time) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (time <= 0)                      return(error(ERR_INVALID_PARAMETER, "invalid parameter time: %d (not positive)", time));

   test->endTime = time;
   return(time);
}


/**
 * Set the bar model used for a test.
 *
 * @param  TEST* test
 * @param  int   type - bar model type
 *
 * @return int - the same bar model type or EMPTY (-1) in case of errors
 */
int WINAPI test_SetBarModel(TEST* test, int type) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));

   switch (type) {
      case BARMODEL_EVERYTICK:
      case BARMODEL_CONTROLPOINTS:
      case BARMODEL_BAROPEN:
         test->barModel = type;
         return(type);
   }
   return(_EMPTY(error(ERR_INVALID_PARAMETER, "invalid parameter type: %d (not a bar model)", type)));
}


/**
 * Set the spread used in a TEST.
 *
 * @param  TEST*  test
 * @param  double spread - spread in pips
 *
 * @return double - the same spread
 */
double WINAPI test_SetSpread(TEST* test, double spread) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (spread < 0)                     return(error(ERR_INVALID_PARAMETER, "invalid parameter spread: %f (must be non-negative)", spread));

   test->spread = round(spread, 1);
   return(test->spread);
}


/**
 * Set the amount of bars in a TEST.
 *
 * @param  TEST* test
 * @param  uint  bars
 *
 * @return uint - the same amount
 */
uint WINAPI test_SetBars(TEST* test, uint bars) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (bars <= 0)                      return(error(ERR_INVALID_PARAMETER, "invalid parameter bars: %d (not positive)", bars));

   test->bars = bars;
   return(bars);
}


/**
 * Set the amount of ticks in a TEST.
 *
 * @param  TEST* test
 * @param  uint  ticks
 *
 * @return uint - the same amount
 */
uint WINAPI test_SetTicks(TEST* test, uint ticks) {
   if ((uint)test < MIN_VALID_POINTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));
   if (ticks <= 0)                     return(error(ERR_INVALID_PARAMETER, "invalid parameter ticks: %d (not positive)", ticks));

   test->ticks = ticks;
   return(ticks);
}


/**
 * Return a human-readable version of a TEST struct.
 *
 * @param  TEST* test
 * @param  BOOL  outputDebug - whether or not to duplicate the result to OutputDebugString()
 *                             (default: no)
 * @return char*
 */
const char* WINAPI TEST_toStr(const TEST* test, BOOL outputDebug/*=FALSE*/) {
   if ((uint)test < MIN_VALID_POINTER) return((char*)error(ERR_INVALID_PARAMETER, "invalid parameter test: 0x%p (not a valid pointer)", test));

   std::stringstream ss;
   TEST empty = {};

   if (!memcmp(test, &empty, sizeof(TEST))) {
      ss << "{(empty)}";
   }
   else {
      ss <<  "{id="              <<                     test->id
         << ", created="         <<                    (test->created   ? doubleQuoteStr(localTimeFormat(test->created, "%a, %d-%b-%Y %H:%M:%S")) : "0")
         << ", strategy="        <<      doubleQuoteStr(test->strategy)
         << ", reportId="        <<                     test->reportId
         << ", reportSymbol="    <<      doubleQuoteStr(test->reportSymbol)
         << ", symbol="          <<      doubleQuoteStr(test->symbol)
         << ", timeframe="       <<      TimeframeToStr(test->timeframe)
         << ", startTime="       <<                    (test->startTime ? doubleQuoteStr(gmtTimeFormat(test->startTime, "%a, %d-%b-%Y %H:%M:%S")) : "0")
         << ", endTime="         <<                    (test->endTime   ? doubleQuoteStr(gmtTimeFormat(test->endTime,   "%a, %d-%b-%Y %H:%M:%S")) : "0")
         << ", barModel="        << BarModelDescription(test->barModel)
         << ", bars="            <<                     test->bars
         << ", ticks="           <<                     test->ticks
         << ", spread="          <<        numberFormat(test->spread, "%.1f")
         << ", tradeDirections=" <<                     test->tradeDirections    // TODO: Long|Short|Both
         << ", orders="          <<                    (test->orders ? to_string(test->orders->size()) : "NULL")
         << "}";
   }
   ss << " (0x" << IntToHexStr((uint)test) << ")";
   char* result = strdup(ss.str().c_str());                                      // TODO: close memory leak

   if (outputDebug) debug(result);
   return(result);
}
