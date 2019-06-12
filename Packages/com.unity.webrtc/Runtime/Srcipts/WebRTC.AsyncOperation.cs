using UnityEngine;

namespace Unity.WebRTC
{

    /// <inheritdoc />
    /// <summary>
    /// </summary>
    public class RTCSessionDescriptionAsyncOperation : CustomYieldInstruction
    {
        public bool isError;
        public RTCError error;
        public RTCSessionDescription desc;
        public bool isDone;
        public override bool keepWaiting
        {
            get
            {
                if (isDone)
                {
                    return false;
                }
                else
                {
                    return true;  
                }   
            }
        }

        internal void Done()
        {
            isDone = true;
        }
    }

    public class RTCIceCandidateRequestAsyncOperation : CustomYieldInstruction
    {
        public bool isError { get; private set;  }
        public RTCError error { get; private set; }
        public bool isDone { get; private set;  }

        public override bool keepWaiting
        {
            get
            {
                return isDone;
            }
        }

        public void Done()
        {
            isDone = true;
        }
    }
    public class RTCAsyncOperation : CustomYieldInstruction
    {
        public bool isError { get; private set; }
        public RTCError error { get; private set; }
        public bool isDone { get; private set; }

        public override bool keepWaiting
        {
            get
            {
                return isDone;
            }
        }

        public void Done()
        {
            isDone = true;
        }
    }
}
