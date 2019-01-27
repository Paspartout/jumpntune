#ifndef SCREENS_H

/* // SCREEN */
/* // ============================================== */
/* void InitSCREEN(Game* game); */
/* void DeinitSCREEN(Game* game); */
/* void UpdateSCREEN(Game* game); */
/* void DrawSCREEN(Game* game); */
/* // ============================================== */

// Intro
// ==============================================
void InitIntro(Game* game);
void DeinitIntro(Game* game);
void UpdateIntro(Game* game);
void DrawIntro(Game* game);
// ==============================================

// Title
// ==============================================
void InitTitle(Game* game);
void DeinitTitle(Game* game);
void UpdateTitle(Game* game);
void DrawTitle(Game* game);
// ==============================================

// Gameplay
// ==============================================
void InitGameplay(Game* game);
void DeinitGameplay(Game* game);
void UpdateGameplay(Game* game);
void DrawGameplay(Game* game);
// ==============================================

// Credits
// ==============================================
void InitCredits(Game* game);
void DeinitCredits(Game* game);
void UpdateCredits(Game* game);
void DrawCredits(Game* game);
// ==============================================

#define SCREENS_H
#endif

