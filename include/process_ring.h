/* process_ring.h*/

extern  process  polling(int32 currentRing,int32*  totalProcess,int32* totalRound,int32* inbox,volatile int * currentElement);
extern process message_passing(pid32 mainPid,pid32* nextProcessPid,int32 roundCount,int32 currentRing);
