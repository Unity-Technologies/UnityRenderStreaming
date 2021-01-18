using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming.RuntimeTest.Signaling
{
    public class MockSignaling : ISignaling
    {
        interface IMockSignalingManager
        {
            void Add(MockSignaling signaling);
            void Remove(MockSignaling signaling);
            void OpenConnection(MockSignaling signaling, string connectionId);
            void CloseConnection(MockSignaling signaling, string connectionId);
            void Offer(MockSignaling owner, ref DescData data);
            void Answer(MockSignaling owner, ref DescData data);
            void Candidate(MockSignaling owner, ref CandidateData data);
        }

        class MockPublicSignalingManager : IMockSignalingManager
        {
            private List<MockSignaling> list = new List<MockSignaling>();

            public void Add(MockSignaling signaling)
            {
                list.Add(signaling);
            }
            public void Remove(MockSignaling signaling)
            {
                list.Remove(signaling);
            }

            public void OpenConnection(MockSignaling signaling, string connectionId)
            {
                signaling.OnCreateConnection?.Invoke(signaling, connectionId, false);
            }

            public void CloseConnection(MockSignaling signaling, string connectionId)
            {
                signaling.OnDestroyConnection?.Invoke(signaling, connectionId);
            }
            public void Offer(MockSignaling owner, ref DescData data)
            {
                foreach (var signaling in list.Where(e => e != owner))
                {
                    signaling.OnOffer?.Invoke(signaling, data);
                }
            }

            public void Answer(MockSignaling owner, ref DescData data)
            {
                foreach (var signaling in list.Where(e => e != owner))
                {
                    signaling.OnAnswer?.Invoke(signaling, data);
                }
            }

            public void Candidate(MockSignaling owner, ref CandidateData data)
            {
                foreach (var signaling in list.Where(e => e != owner))
                {
                    signaling.OnIceCandidate?.Invoke(signaling, data);
                }
            }
        }

        class MockPrivateSignalingManager : IMockSignalingManager
        {
            private Dictionary<string, List<MockSignaling>> connectionIds = new Dictionary<string, List<MockSignaling>>();

            public void Add(MockSignaling signaling)
            {
            }
            public void Remove(MockSignaling signaling)
            {
            }
            public void OpenConnection(MockSignaling signaling, string connectionId)
            {
                bool peerExists = connectionIds.TryGetValue(connectionId, out var list);
                if(!peerExists)
                {
                    list = new List<MockSignaling>();
                    connectionIds.Add(connectionId, list);
                }
                list.Add(signaling);
                signaling.OnCreateConnection?.Invoke(signaling, connectionId, peerExists);
            }

            public void CloseConnection(MockSignaling signaling, string connectionId)
            {
                bool peerExists = connectionIds.TryGetValue(connectionId, out var list);
                if (!peerExists || !list.Contains(signaling))
                {
                    Debug.LogError($"{connectionId} This connection id is not used.");
                }
                list.Remove(signaling);
                if (list.Count == 0)
                {
                    connectionIds.Remove(connectionId);
                }
                signaling.OnDestroyConnection?.Invoke(signaling, connectionId);
            }

            List<MockSignaling> FindList(MockSignaling owner, string connectionId)
            {
                if (!connectionIds.TryGetValue(connectionId, out var list))
                {
                    return null;
                }
                list = list.Where(e => e != owner).ToList();
                if (list.Count == 0)
                {
                    return null;
                }
                return list;
            }

            public void Offer(MockSignaling owner, ref DescData data)
            {
                var list = FindList(owner, data.connectionId);
                if (list == null)
                {
                    Debug.LogError($"{data.connectionId} This connection id is not ready other session.");
                    return;
                }
                foreach (var signaling in list)
                {
                    signaling.OnOffer?.Invoke(signaling, data);
                }
            }

            public void Answer(MockSignaling owner, ref DescData data)
            {
                var list = FindList(owner, data.connectionId);
                if (list == null)
                {
                    Debug.LogError($"{data.connectionId} This connection id is not ready other session.");
                    return;
                }
                foreach (var signaling in list)
                {
                    signaling.OnAnswer?.Invoke(signaling, data);
                }
            }

            public void Candidate(MockSignaling owner, ref CandidateData data)
            {
                var list = FindList(owner, data.connectionId);
                if (list == null)
                {
                    Debug.LogError($"{data.connectionId} This connection id is not ready other session.");
                    return;
                }
                foreach (var signaling in list.Where(e => e != owner))
                {
                    signaling.OnIceCandidate?.Invoke(signaling, data);
                }
            }
        }

        private static IMockSignalingManager manager = null;

        public static void Reset(bool enablePrivateMode)
        {
            if (enablePrivateMode)
            {
                manager = new MockPrivateSignalingManager();
            }
            else
            {
                manager = new MockPublicSignalingManager();
            }
        }

        static MockSignaling()
        {
            manager = new MockPublicSignalingManager();
        }

        public MockSignaling()
        {
        }

        public void Start()
        {
            manager.Add(this);
            this.OnStart?.Invoke(this);
        }

        public void Stop()
        {
            manager.Remove(this);
        }

        public event OnStartHandler OnStart;
        public event OnConnectHandler OnCreateConnection;
        public event OnDisconnectHandler OnDestroyConnection;
        public event OnOfferHandler OnOffer;
        public event OnAnswerHandler OnAnswer;
        public event OnIceCandidateHandler OnIceCandidate;

        public void OpenConnection(string connectionId)
        {
            manager.OpenConnection(this, connectionId);
        }

        public void CloseConnection(string connectionId)
        {
            manager.CloseConnection(this, connectionId);
        }

        public void SendOffer(string connectionId, RTCSessionDescription offer)
        {
            DescData data = new DescData
            {
                connectionId = connectionId,
                type = "offer",
                sdp = offer.sdp
            };
            manager.Offer(this, ref data);
        }

        public void SendAnswer(string connectionId, RTCSessionDescription answer)
        {
            DescData data = new DescData
            {
                connectionId = connectionId,
                type = "answer",
                sdp = answer.sdp
            };
            manager.Answer(this, ref data);
        }

        public void SendCandidate(string connectionId, RTCIceCandidate candidate)
        {
            CandidateData data = new CandidateData
            {
                connectionId = connectionId,
                candidate = candidate.Candidate,
                sdpMid = candidate.SdpMid,
                sdpMLineIndex = candidate.SdpMLineIndex.GetValueOrDefault()
            };
            manager.Candidate(this, ref data);
        }
    }
}
