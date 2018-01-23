/* process_ring.h*/

extern process polling(int32* currentProcess,int32* nextProcess,int32 currentRing,int32 currentRound);
extern process message_passing(pid32 mainPid,pid32* nextProcessPid,int32 roundCount,int32 currentRing);
