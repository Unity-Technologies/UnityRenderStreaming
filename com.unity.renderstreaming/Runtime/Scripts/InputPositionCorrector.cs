using System;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming
{
    class InputPositionCorrector
    {
        public Rect inputRegion { set; get; }

        public Rect outputRegion { set; get; }

        private Action<InputEventPtr, InputDevice> _onEvent;

        public InputPositionCorrector(Action<InputEventPtr, InputDevice> onEvent)
        {
            _onEvent = onEvent;
        }

        public unsafe void Invoke(InputEventPtr ptr, InputDevice device)
        {
            // Allocate memory and copy InputEventPtr
            InputEventPtr dst = (InputEventPtr)
                UnsafeUtility.Malloc(ptr.sizeInBytes, 4, Collections.Allocator.Temp);
            UnsafeUtility.MemCpy(dst, ptr, ptr.sizeInBytes);

            // Mapping 
            PointerMap((StateEvent*)dst.data, device);

            _onEvent?.Invoke(dst, device);

            // Free memory
            UnsafeUtility.Free(dst, Collections.Allocator.Temp);
        }

        unsafe void PointerMap(StateEvent* data, InputDevice device)
        {
            switch (device)
            {
                case Mouse mouse:
                    MouseState* mouseState = (MouseState*)data->state;
                    mouseState->position = Map(mouseState->position, inputRegion, outputRegion);
                    break;
                case Touchscreen touch:
                    // todo(kazuki): multi touch is not supported yet.
                    TouchState* touchState = (TouchState*)data->state;
                    touchState->position = Map(touchState->position, inputRegion, outputRegion);
                    break;
            }
        }

        static Vector2 Map(Vector2 pos, Rect inputRegion, Rect outputRegion)
        {
            Vector2 normalized = Rect.PointToNormalized(inputRegion, pos);
            return Rect.NormalizedToPoint(outputRegion, normalized);
        }
    }
}
