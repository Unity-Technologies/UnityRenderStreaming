using System.Runtime.InteropServices;
using System;

namespace Unity.WebRTC
{

    public class RTCDataChannel
    {
        private IntPtr self;
        private DelegateOnMessage onMessage;
        private DelegateOnOpen onOpen;
        private DelegateOnClose onClose;
        public int id;
        public string label;

        public DelegateOnMessage OnMessage
        {
            get { return onMessage; }
            set
            {
                onMessage = value;
                NativeMethods.DataChannelRegisterOnMessage(self, onMessage);
            }
        }

        public DelegateOnOpen OnOpen
        {
            get { return onOpen; }
            set
            {
                onOpen = value;
                NativeMethods.DataChannelRegisterOnOpen(self, onOpen);
            }
        }
        public DelegateOnClose OnClose
        {
            get { return onClose; }
            set
            {
                onClose = value;
                NativeMethods.DataChannelRegisterOnClose(self, onClose);
            }
        }

        public RTCDataChannel(IntPtr ptr)
        {
            self = ptr;
            id = NativeMethods.DataChannelGetID(self);
            IntPtr labelPtr = NativeMethods.DataChannelGetLabel(self);
            label = Marshal.PtrToStringAnsi(labelPtr);
        }
        public void Send(string msg)
        {
            NativeMethods.DataChannelSend(self, msg);
        }

        public void Close()
        {
            NativeMethods.DataChannelClose(self);
        }
        public void ActivateCallback()
        {
            NativeMethods.DataChannelRegisterObserver(self);
        }
        public void DeactivateCallback()
        {
            NativeMethods.DataChannelUnregisterObserver(self);
        }
    }
}
