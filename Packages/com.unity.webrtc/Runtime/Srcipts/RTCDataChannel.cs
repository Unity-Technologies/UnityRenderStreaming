using System.Runtime.InteropServices;
using System;

namespace Unity.WebRTC
{
    public delegate void DelegateOnMessage(byte[] bytes);
    public delegate void DelegateOnDataChannel(RTCDataChannel channel);

    public class RTCDataChannel
    {
        private IntPtr self;
        private DelegateOnMessage onMessage;
        private DelegateOnOpen onOpen;
        private DelegateOnClose onClose;

        private DelegateNativeOnMessage selfOnMessage;
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
                selfOnMessage = new DelegateNativeOnMessage(DataChannelNativeOnMessage);
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

        public int Id
        {
            get => NativeMethods.DataChannelGetID(self);
        }
        public string Label { get => label; private set => label = value; }

        void DataChannelNativeOnMessage(byte[] msg, int len)
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
            var labelPtr = NativeMethods.DataChannelGetLabel(self);
            Label = Marshal.PtrToStringAnsi(labelPtr);
        }
        public void Send(string msg)
        {
            NativeMethods.DataChannelSend(self, msg);
        }

        public void Send(byte[] msg)
        {
            NativeMethods.DataChannelSendBinary(self, msg, msg.Length);
        }

        public void Close()
        {
            NativeMethods.DataChannelClose(self);
        }
    }
}
