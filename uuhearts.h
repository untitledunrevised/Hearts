#ifndef	UU_HEARTS_H
#define	UU_HEARTS_H

#define DLLEXPORT __declspec(dllexport)

typedef int BOOL;
#define TRUE 1
#define FALSE 0

extern "C" DLLEXPORT void uu_hearts_init();
extern "C" DLLEXPORT void uu_hearts_start();
extern "C" DLLEXPORT BOOL uu_heart_need_exchange();
extern "C" DLLEXPORT BOOL uu_heart_exchange_request(const int playerid, const int firstcard, const int secondcard, const int thirdcard);
extern "C" DLLEXPORT void uu_heart_exchange();
extern "C" DLLEXPORT BOOL uu_heart_playcard(const int card);
extern "C" DLLEXPORT BOOL uu_heart_roundover();
extern "C" DLLEXPORT BOOL uu_heart_gameover();
extern "C" DLLEXPORT int getscore(const int roundid, const int playerid);

#endif
