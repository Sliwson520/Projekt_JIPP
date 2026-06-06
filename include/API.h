
#ifndef API_H
#define API_H

#pragma once

#if defined(_BUILD) || defined(GAMEENGINE_EXPORTS)

#define ENGINE_API __declspec(dllexport)
#else

#define ENGINE_API __declspec(dllimport)
#endif

#endif