#include "expander.h"
#include "lib/datetime.h"
#include "lib/helper.h"

#include <time.h>
#include <vector>


/**
 * Return the system's current GMT time (also in Strategy Tester).
 *
 * @return datetime - Unix timestamp as seconds since 01.01.1970 00:00:00 GMT
 */
datetime WINAPI GetGmtTime() {
   return(time(NULL));
   #pragma EXPANDER_EXPORT
}


/**
 * Return the system's current local time (also in Strategy Tester).
 *
 * @return datetime - Unix timestamp as seconds since 01.01.1970 00:00:00 local time
 */
datetime WINAPI GetLocalTime() {
   // @see  MSDN for manual replacement of the non-public function RtlTimeToSecondsSince1970()
   SYSTEMTIME     stNow,    st1970 = {1970, 1, 4, 1, 0, 0, 0, 0};
   FILETIME       ftNow,    ft1970;
   ULARGE_INTEGER ulintNow, ulint1970;

   GetLocalTime(&stNow);
   if (!SystemTimeToFileTime(&stNow, &ftNow))   return(error(ERR_WIN32_ERROR+GetLastError(), "=> SystemTimeToFileTime()"));
   ulintNow.LowPart   = ftNow.dwLowDateTime;
   ulintNow.HighPart  = ftNow.dwHighDateTime;

   if (!SystemTimeToFileTime(&st1970, &ft1970)) return(error(ERR_WIN32_ERROR+GetLastError(), "=> SystemTimeToFileTime()"));
   ulint1970.LowPart  = ft1970.dwLowDateTime;
   ulint1970.HighPart = ft1970.dwHighDateTime;

   datetime secondsSince1970 = (datetime)((ulintNow.QuadPart - ulint1970.QuadPart)/10000000);
   return(secondsSince1970);
   #pragma EXPANDER_EXPORT
}


std::vector<TICK_TIMER_DATA> tickTimers;                             // Daten aller aktiven TickTimer


/**
 * Callback function for WM_TIMER messages.
 *
 * @param  HWND     hWnd    - Handle to the window associated with the timer.
 * @param  UINT     msg     - Specifies the WM_TIMER message.
 * @param  UINT_PTR timerId - Specifies the timer's identifier.
 * @param  DWORD    time    - Specifies the number of milliseconds that have elapsed since the system was started. This is the
 *                            value returned by the GetTickCount() function.
 */
VOID CALLBACK TimerCallback(HWND hWnd, UINT msg, UINT_PTR timerId, DWORD time) {
   int size = tickTimers.size();

   for (int i=0; i < size; i++) {
      if (tickTimers[i].id == timerId) {
         TICK_TIMER_DATA& ttd = tickTimers[i];

         if (ttd.flags & TICK_IF_VISIBLE) {
            RECT rect;                                   // check if chart is (partially) visible
            HDC hDC = GetDC(hWnd);
            int rgn = GetClipBox(hDC, &rect);
            ReleaseDC(hWnd, hDC);

            if (rgn == NULLREGION)                       // skip timer event if chart is not visible
               return;
            if (rgn == RGN_ERROR) {
               warn(ERR_WIN32_ERROR+GetLastError(), "GetClipBox(hDC=%p) => RGN_ERROR", hDC);
               return;
            }
         }
         if (ttd.flags & TICK_PAUSE_ON_WEEKEND) {
            // skip timer event if on weekend: not yet implemented
         }

         if (ttd.flags & TICK_CHART_REFRESH) {
            PostMessageA(hWnd, WM_COMMAND, ID_CHART_REFRESH, 0);
         }
         else if (ttd.flags & TICK_TESTER) {
            PostMessageA(hWnd, WM_COMMAND, ID_CHART_STEPFORWARD, 0);
         }
         else {
            PostMessageA(hWnd, MT4InternalMsg(), MT4_TICK, TICK_OFFLINE_EA);  // default tick
         }
         return;
      }
   }
   warn(ERR_RUNTIME_ERROR, "timer not found, timerId: %d", timerId);
}


/**
 * Installiert einen Timer, der dem angegebenen Fenster synthetische Ticks schickt.
 *
 * @param  HWND  hWnd   - Handle des Fensters, an das die Ticks geschickt werden.
 * @param  int   millis - Zeitperiode der zu generierenden Ticks in Millisekunden
 * @param  DWORD flags  - die Ticks konfigurierende Flags (default: keine)
 *                        TICK_CHART_REFRESH:    Statt eines regul�ren Ticks wird das Command ID_CHART_REFRESH an den Chart
 *                                               geschickt (f�r Offline- und synthetische Charts).
 *                        TICK_TESTER:           Statt eines regul�ren Ticks wird das Command ID_CHART_STEPFORWARD an den Chart
 *                                               geschickt (f�r Strategy Tester).
 *                        TICK_IF_VISIBLE:       Ticks werden nur verschickt, wenn der Chart mindestens teilweise sichtbar ist.
 *                        TICK_PAUSE_ON_WEEKEND: Ticks werden nur zu regul�ren Forex-Handelszeiten verschickt (not implemented).
 *
 * @return uint - ID des installierten Timers zur �bergabe an RemoveTickTimer() bei Deinstallation des Timers oder 0, falls ein
 *                Fehler auftrat.
 */
uint WINAPI SetupTickTimer(HWND hWnd, int millis, DWORD flags/*=NULL*/) {
   // Parametervalidierung
   DWORD wndThreadId = GetWindowThreadProcessId(hWnd, NULL);
   if (wndThreadId != GetCurrentThreadId()) {
      if (!wndThreadId)                                   return(error(ERR_INVALID_PARAMETER, "invalid parameter hWnd: %p (not a window)", hWnd));
                                                          return(error(ERR_INVALID_PARAMETER, "window hWnd=%p not owned by the current thread", hWnd));
   }
   if (millis <= 0)                                       return(error(ERR_INVALID_PARAMETER, "invalid parameter millis: %d", millis));
   if (flags & TICK_CHART_REFRESH && flags & TICK_TESTER) return(error(ERR_INVALID_PARAMETER, "invalid parameter flags combination: TICK_CHART_REFRESH & TICK_TESTER"));
   if (flags & TICK_PAUSE_ON_WEEKEND)                     warn(ERR_NOT_IMPLEMENTED, "flag TICK_PAUSE_ON_WEEKEND not yet implemented");

   // neue Timer-ID erzeugen
   static uint timerId = 10000;                       // ID's sind mindestens 5-stellig und beginnen bei 10000
   timerId++;

   // Timer setzen
   uint result = SetTimer(hWnd, timerId, millis, (TIMERPROC)TimerCallback);
   if (result != timerId)                             // mu� stimmen, da hWnd immer != NULL
      return(error(ERR_WIN32_ERROR+GetLastError(), "=> SetTimer(hWnd=%p, timerId=%d, millis=%d) => %d", hWnd, timerId, millis, result));
   //debug("SetTimer(hWnd=%d, timerId=%d, millis=%d) success", hWnd, timerId, millis);

   // Timerdaten speichern
   TICK_TIMER_DATA ttd = {timerId, hWnd, flags};
   tickTimers.push_back(ttd);                         // TODO: avoid push_back() creating a copy

   return(timerId);
   #pragma EXPANDER_EXPORT
}


/**
 * Deinstalliert einen mit SetupTickTimer() installierten Timer.
 *
 * @param  int timerId - ID des Timers, wie von SetupTickTimer() zur�ckgegeben.
 *
 * @return BOOL - Erfolgsstatus
 */
BOOL WINAPI RemoveTickTimer(int timerId) {
   if (timerId <= 0) return(error(ERR_INVALID_PARAMETER, "invalid parameter timerId: %d", timerId));
   int size = tickTimers.size();

   for (int i=0; i < size; i++) {
      if (tickTimers[i].id == timerId) {
         if (!KillTimer(tickTimers[i].hWnd, timerId))
            return(error(ERR_WIN32_ERROR+GetLastError(), "=> KillTimer(hWnd=%p, timerId=%d)", tickTimers[i].hWnd, timerId));
         tickTimers.erase(tickTimers.begin() + i);
         return(TRUE);
      }
   }

   return(error(ERR_RUNTIME_ERROR, "timer not found, id: %d", timerId));
   #pragma EXPANDER_EXPORT
}


/**
 * Deinstalliert alle mit SetupTickTimer() installierten Timer, die nicht explizit deinstalliert wurden.
 * Wird in onProcessDetach() aufgerufen.
 */
void WINAPI RemoveTickTimers() {
   int size = tickTimers.size();

   for (int i=size-1; i>=0; i--) {                 // r�ckw�rts, da der Vector in RemoveTickTimer() modifiziert wird
      uint id = tickTimers[i].id;
      warn(NO_ERROR, "removing orphaned tickTimer with id: %d", id);
      RemoveTickTimer(id);
   }
}
