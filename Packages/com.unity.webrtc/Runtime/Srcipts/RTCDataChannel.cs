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

        private DelegateOnMessage selfOnMessage;
        private DelegateOnOpen selfOnOpen;
        private DelegateOnClose selfOnClose;
        private int id;
        private string label;

        public DelegateOnMessage OnMessage
        {
            get { return onMessage; }
            set
            {
                onMessage = value;
                selfOnMessage = new DelegateOnMessage(DataChannelOnMessage);
                NativeMethods.DataChannelRegisterOnMessage(self, selfOnMessage);
            }
        }

        public DelegateOnOpen OnOpen
        {
            get { return onOpen; }
            set
            {
                onOpen = value;
                selfOnOpen = new DelegateOnOpen(DataChannelOnOpen);
                NativeMethods.DataChannelRegisterOnOpen(self, selfOnOpen);
            }
        }
        public DelegateOnClose OnClose
        {
            get { return onClose; }
            set
            {
                onClose = value;
                selfOnClose = new DelegateOnClose(DataChannelOnClose);
                NativeMethods.DataChannelRegisterOnClose(self, selfOnClose);
            }
        }

        public int Id { get => id; private set => id = value; }
        public string Label { get => label; private set => label = value; }

        void DataChannelOnMessage(string msg)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                onMessage(msg);
            }, null);
        }
        void DataChannelOnOpen()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                onOpen();
            }, null);
        }
        void DataChannelOnClose()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                onClose();
            }, null);
        }
        internal RTCDataChannel(IntPtr ptr)
        {
            self = ptr;
            Id = NativeMethods.DataChannelGetID(self);
            IntPtr labelPtr = NativeMethods.DataChannelGetLabel(self);
            Label = Marshal.PtrToStringAnsi(labelPtr);
        }
        public void Send(string msg)
        {
            NativeMethods.DataChannelSend(self, msg);
        }

        public void Close()
        {
            NativeMethods.DataChannelClose(self);
        }
    }
}
