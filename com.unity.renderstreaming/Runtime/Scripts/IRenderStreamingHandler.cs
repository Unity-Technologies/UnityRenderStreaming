using System;
using System.Collections.Generic;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    public interface IRenderStreamingDelegate
    {
        /// <summary>
        ///
        /// </summary>
        event Action onStart;

        /// <summary>
        ///
        /// </summary>
        event Action<string> onCreatedConnection;

        /// <summary>
        ///
        /// </summary>
        event Action<string> onDeletedConnection;

        /// <summary>
        ///
        /// </summary>
        event Action<string, string> onGotOffer;

        /// <summary>
        ///
        /// </summary>
        event Action<string, string> onGotAnswer;

        /// <summary>
        ///
        /// </summary>
        event Action<string> onConnect;

        /// <summary>
        ///
        /// </summary>
        event Action<string> onDisconnect;

        /// <summary>
        ///
        /// </summary>
        event Action<string, RTCRtpTransceiver> onAddTransceiver;

        /// <summary>
        ///
        /// </summary>
        event Action<string, RTCDataChannel> onAddChannel;
    }

    public interface IRenderStreamingHandler
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        void CreateConnection(string connectionId);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        void DeleteConnection(string connectionId);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        bool ExistConnection(string connectionId);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        bool IsConnected(string connectionId);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connection"></param>
        /// <returns></returns>
        bool IsStable(string connection);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="label"></param>
        /// <returns></returns>
        RTCDataChannel CreateChannel(string connectionId, string label = null);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        void SendOffer(string connectionId);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        void SendAnswer(string connectionId);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <param name="init"></param>
        /// <returns></returns>
        RTCRtpTransceiver AddTransceiver(string connectionId, MediaStreamTrack track, RTCRtpTransceiverInit init = null);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="kind"></param>
        /// <param name="init"></param>
        /// <returns></returns>
        RTCRtpTransceiver AddTransceiver(string connectionId, TrackKind kind, RTCRtpTransceiverInit init = null);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        void RemoveSenderTrack(string connectionId, MediaStreamTrack track);

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <returns></returns>
        IEnumerable<RTCRtpTransceiver> GetTransceivers(string connectionId);
    }
}
