#pragma once

#include "Vector.h"

namespace Engine::Rendering
{
    class Camera final
    {
    public:
        void Set() const;

    public:
        Vector<2> Location = { 0.0f, 0.0f };
        Vector<2> Sight    = { 0.0f, 0.0f };
        float     Angle    = { 0.0f };
    };

    namespace Text
    {
        class Component final
        {
        public:
            void Render();

        public:
            char const * Text = nullptr;

            struct
            {
                char const * Name       = nullptr;
                int          Size       = 0;
                bool         Bold       = false;
                bool         Italic     = false;
                bool         Underlined = false;
                bool         StructOut  = false;
            }
            Font;

            struct
            {
                unsigned char Red   = 0;
                unsigned char Green = 0;
                unsigned char Blue  = 0;
            }
            Color;

        public:
            Vector<2> Location = { 0.0f, 0.0f };
            Vector<2> Length   = { 0.0f, 0.0f };
            float     Angle    = { 0.0f };
        };
    }

    namespace Image
    {
        class Component final
        {
        public:
            void Render();

        public:
            char const * Name = nullptr;

        public:
            Vector<2> Location = { 0.0f, 0.0f };
            Vector<2> Length   = { 0.0f, 0.0f };
            float     Angle    = { 0.0f };
        };

        class Test final
        {
        public:
            void Render();

        public:
            char const * Name = nullptr;

        public:
            Vector<2> Location = { 0.0f, 0.0f };
            Vector<2> Length   = { 0.0f, 0.0f };
            float     Angle    = { 0.0f };
        };
    }

    namespace Animation
    {
        class Component final
        {
        public:
            void Render();

        public:
            char const * Name       = nullptr;
            float        Playback   = 0.0f;
            float        Duration   = 0.0f;
            bool         Repeatable = false;
            bool         Flipped    = false;

        public:
            Vector<2> Location = { 0.0f, 0.0f };
            Vector<2> Length   = { 0.0f, 0.0f };
            float     Angle    = { 0.0f };
        };
    }
}