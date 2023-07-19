// note:: This script is using code snippets in InputSystem.
// https://github.com/Unity-Technologies/InputSystem/blob/develop/Packages/com.unity.inputsystem/InputSystem/Devices/Remote/InputRemoting.cs
using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

////TODO: show remote device IDs in the debugger

////TODO: remote timestamps need to be translated to local timestamps; doesn't make sense for remote events getting
////      processed on the local timeline as is when the originating timeline may be quite different

////TODO: support actions

////TODO: support input users

////TODO: text input events

////REVIEW: it seems that the various XXXMsg struct should be public; ATM doesn't seem like working with the message interface is practical

////REVIEW: the namespacing mechanism for layouts which changes base layouts means that layouts can't be played
////        around with on the editor side but will only be changed once they're updated in the player

namespace Unity.RenderStreaming.InputSystem
{
    /// <summary>
    /// Makes the activity and data of an InputManager observable in message form.
    /// </summary>
    /// <remarks>
    /// Can act as both the sender and Receiver of these message so the flow is fully bidirectional,
    /// i.e. the InputManager on either end can mirror its layouts, devices, and events over
    /// to the other end. This permits streaming input not just from the player to the editor but
    /// also feeding input from the editor back into the player.
    ///
    /// Remoting sits entirely on top of the input system as an optional piece of functionality.
    /// In development players and the editor, we enable it automatically but in non-development
    /// players it has to be explicitly requested by the user.
    ///
    /// To see devices and input from players in the editor, open the Input Debugger through
    /// "Windows >> Input Debugger".
    /// </remarks>
    /// <seealso cref="InputSystem.remoting"/>
    /// \todo Reuse memory allocated for messages instead of allocating separately for each message.
    /// \todo Inteface to determine what to mirror from the local manager to the remote system.
    public sealed class InputRemoting : IObservable<InputRemoting.Message>, IObserver<InputRemoting.Message>
    {
        /// <summary>
        /// Enumeration of possible types of messages exchanged between two InputRemoting instances.
        /// </summary>
        public enum MessageType
        {
            Connect,
            Disconnect,
            NewLayout,
            NewDevice,
            NewEvents,
            RemoveDevice,
            RemoveLayout,
            ChangeUsages,
            StartSending,
            StopSending,
        }

        /// <summary>
        /// A message exchanged between two InputRemoting instances.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct Message
        {
            /// <summary>
            /// For messages coming in, numeric ID of the sender of the message. For messages
            /// going out, numeric ID of the targeted Receiver of the message.
            /// </summary>
            public int participantId;
            public MessageType type;
            public byte[] data;
        }

        public bool sending
        {
            get => (m_Flags & Flags.Sending) == Flags.Sending;
            private set
            {
                if (value)
                    m_Flags |= Flags.Sending;
                else
                    m_Flags &= ~Flags.Sending;
            }
        }

        static InputRemoting()
        {
#if UNITY_EDITOR
            //
            // note: This lines are for avoiding issues when running the editor
            // on background. When moved the focus from the editor, input events
            // from another process are ignored.
            // Please attention behaviours are difference several platforms when
            // moving focus from Unity Editor.
            //
            // Additionally, The behaviour is changed Unity2021.2 later (using
            // InputSystem 1.1). Please see "Background behaviour".
            // https://docs.unity3d.com/Packages/com.unity.inputsystem@1.1/manual/Settings.html#background-behavior
#if INPUTSYSTEM_1_1_OR_NEWER
            // todo(kazuki):
#else
            // Make sure we're not affected by the user giving focus away from the
            // game view.
            //
            InputEditorUserSettings.lockInputToGameView = true;
#endif
#endif
        }

        internal InputRemoting(IInputManager manager, bool startSendingOnConnect = false)
        {
            if (manager == null)
                throw new ArgumentNullException("manager");

            m_LocalManager = manager;

            if (startSendingOnConnect)
                m_Flags |= Flags.StartSendingOnConnect;

            //when listening for newly added layouts, must filter out ones we've added from remote
        }

        /// <summary>
        /// Start sending messages for data and activity in the local input system
        /// to observers.
        /// </summary>
        /// <seealso cref="sending"/>
        /// <seealso cref="StopSending"/>
        public void StartSending()
        {
            if (sending)
                return;

            ////TODO: send events in bulk rather than one-by-one
            m_LocalManager.onMessage += Send;
            m_LocalManager.onEvent += SendEvent;
            m_LocalManager.onDeviceChange += SendDeviceChange;
            m_LocalManager.onLayoutChange += SendLayoutChange;

            sending = true;

            SendInitialMessages();
        }

        public void StopSending()
        {
            if (!sending)
                return;

            m_LocalManager.onMessage -= Send;
            m_LocalManager.onEvent -= SendEvent;
            m_LocalManager.onDeviceChange -= SendDeviceChange;
            m_LocalManager.onLayoutChange -= SendLayoutChange;

            sending = false;
        }

        void IObserver<Message>.OnNext(Message msg)
        {
            switch (msg.type)
            {
                case MessageType.Connect:
                    ConnectMsg.Process(this);
                    break;
                case MessageType.Disconnect:
                    DisconnectMsg.Process(this, msg);
                    break;
                case MessageType.NewLayout:
                    NewLayoutMsg.Process(this, msg);
                    break;
                case MessageType.RemoveLayout:
                    RemoveLayoutMsg.Process(this, msg);
                    break;
                case MessageType.NewDevice:
                    NewDeviceMsg.Process(this, msg);
                    break;
                case MessageType.NewEvents:
                    NewEventsMsg.Process(this, msg);
                    break;
                case MessageType.ChangeUsages:
                    ChangeUsageMsg.Process(this, msg);
                    break;
                case MessageType.RemoveDevice:
                    RemoveDeviceMsg.Process(this, msg);
                    break;
                case MessageType.StartSending:
                    StartSendingMsg.Process(this);
                    break;
                case MessageType.StopSending:
                    StopSendingMsg.Process(this);
                    break;
            }
        }

        void IObserver<Message>.OnError(Exception error)
        {
        }

        void IObserver<Message>.OnCompleted()
        {
        }

        public IDisposable Subscribe(IObserver<Message> observer)
        {
            if (observer == null)
                throw new ArgumentNullException("observer");

            var subscriber = new Subscriber { owner = this, observer = observer };
            ArrayHelpers.Append(ref m_Subscribers, subscriber);

            return subscriber;
        }

        private void SendInitialMessages()
        {
            SendAllGeneratedLayouts();
            SendAllDevices();
        }

        private void SendAllGeneratedLayouts()
        {
            // todo(kazuki)::
            // layputBuilders property is not published from InputSystem
            //
            //foreach (var entry in m_LocalManager.m_Layouts.layoutBuilders)
            //    SendLayout(entry.Key);

            foreach (var layout in m_LocalManager.layouts)
                SendLayout(layout);
        }

        private void SendLayout(string layoutName)
        {
            if (m_Subscribers == null)
                return;

            var message = NewLayoutMsg.Create(this, layoutName);
            if (message != null)
                Send(message.Value);
        }

        private void SendAllDevices()
        {
            var devices = m_LocalManager.devices;
            foreach (var device in devices)
                SendDevice(device);
        }

        private void SendDevice(InputDevice device)
        {
            if (m_Subscribers == null)
                return;

            // Don't mirror remote devices to other remotes.
            if (device.remote)
                return;

            var newDeviceMessage = NewDeviceMsg.Create(device);
            Send(newDeviceMessage);

            // Send current state. We do this here in this case as the device
            // may have been added some time ago and thus have already received events.
            var stateEventMessage = NewEventsMsg.CreateStateEvent(device);
            Send(stateEventMessage);
        }

        private unsafe void SendEvent(InputEventPtr eventPtr, InputDevice device)
        {
            if (m_Subscribers == null)
                return;

            ////REVIEW: we probably want to have better control over this and allow producing local events
            ////        against remote devices which *are* indeed sent across the wire
            // Don't send events that came in from remote devices.
            if (device != null && device.remote)
                return;

            var message = NewEventsMsg.Create(eventPtr.data, 1);
            Send(message);
        }

        private void SendDeviceChange(InputDevice device, InputDeviceChange change)
        {
            if (m_Subscribers == null)
                return;

            // Don't mirror remoted devices to other remotes.
            if (device.remote)
                return;

            Message msg;
            switch (change)
            {
                case InputDeviceChange.Added:
                    msg = NewDeviceMsg.Create(device);
                    break;
                case InputDeviceChange.Removed:
                    msg = RemoveDeviceMsg.Create(device);
                    break;
                case InputDeviceChange.UsageChanged:
                    msg = ChangeUsageMsg.Create(device);
                    break;
                default:
                    return;
            }

            Send(msg);
        }

        private void SendLayoutChange(string layout, InputControlLayoutChange change)
        {
            if (m_Subscribers == null)
                return;

            Message msg;
            switch (change)
            {
                case InputControlLayoutChange.Added:
                case InputControlLayoutChange.Replaced:
                    var message = NewLayoutMsg.Create(this, layout);
                    if (message == null)
                        return;
                    msg = message.Value;
                    break;
                case InputControlLayoutChange.Removed:
                    msg = RemoveLayoutMsg.Create(layout);
                    break;
                default:
                    return;
            }

            Send(msg);
        }

        private void Send(Message msg)
        {
            foreach (var subscriber in m_Subscribers)
                subscriber.observer.OnNext(msg);
        }

        ////TODO: with C#7 this should be a ref return
        private int FindOrCreateSenderRecord(int senderId)
        {
            // Try to find existing.
            if (m_Senders != null)
            {
                var senderCount = m_Senders.Length;
                for (var i = 0; i < senderCount; ++i)
                    if (m_Senders[i].senderId == senderId)
                        return i;
            }

            // Create new.
            var sender = new RemoteSender
            {
                senderId = senderId,
            };
            return ArrayHelpers.Append(ref m_Senders, sender);
        }

        private int FindLocalDeviceId(int remoteDeviceId, int senderIndex)
        {
            var localDevices = m_Senders[senderIndex].devices;
            if (localDevices != null)
            {
                var numLocalDevices = localDevices.Length;

                for (var i = 0; i < numLocalDevices; ++i)
                {
                    if (localDevices[i].remoteId == remoteDeviceId)
                        return localDevices[i].localId;
                }
            }

            return InputDevice.InvalidDeviceId;
        }

        private InputDevice TryGetDeviceByRemoteId(int remoteDeviceId, int senderIndex)
        {
            var localId = FindLocalDeviceId(remoteDeviceId, senderIndex);
            return m_LocalManager.GetDeviceById(localId);
        }

        private Flags m_Flags;
        private IInputManager m_LocalManager; // Input system we mirror input from and to.
        private Subscriber[] m_Subscribers; // Receivers we send input to.
        private RemoteSender[] m_Senders; // Senders we receive input from.

        [Flags]
        private enum Flags
        {
            Sending = 1 << 0,
            StartSendingOnConnect = 1 << 1
        }

        // Data we keep about a unique sender that we receive input data
        // from. We keep track of the layouts and devices we added to
        // the local system.
        [Serializable]
        internal struct RemoteSender
        {
            public int senderId;
            public string[] layouts;
            public RemoteInputDevice[] devices;
        }

        [Serializable]
        internal struct RemoteInputDevice
        {
            public int remoteId; // Device ID used by sender.
            public int localId; // Device ID used by us in local system.

            public InputDeviceDescription description;
        }

        internal class Subscriber : IDisposable
        {
            public InputRemoting owner;
            public IObserver<Message> observer;
            public void Dispose()
            {
                ArrayHelpers.Erase(ref owner.m_Subscribers, this);
            }
        }

        private static class ConnectMsg
        {
            public static void Process(InputRemoting Receiver)
            {
                if (Receiver.sending)
                {
                    Receiver.SendAllDevices();
                }
                else if ((Receiver.m_Flags & Flags.StartSendingOnConnect) == Flags.StartSendingOnConnect)
                    Receiver.StartSending();
            }
        }

        private static class StartSendingMsg
        {
            public static void Process(InputRemoting Receiver)
            {
                Receiver.StartSending();
            }
        }

        private static class StopSendingMsg
        {
            public static void Process(InputRemoting Receiver)
            {
                Receiver.StopSending();
            }
        }

        public void RemoveRemoteDevices(int participantId)
        {
            var senderIndex = FindOrCreateSenderRecord(participantId);

            // Remove devices added by remote.
            var devices = m_Senders[senderIndex].devices;
            if (devices != null)
            {
                foreach (var remoteDevice in devices)
                {
                    var device = m_LocalManager.GetDeviceById(remoteDevice.localId);
                    if (device != null)
                        m_LocalManager.RemoveDevice(device);
                }
            }

            ////TODO: remove layouts added by remote
            ArrayHelpers.EraseAt(ref m_Senders, senderIndex);
        }

        private static class DisconnectMsg
        {
            public static void Process(InputRemoting Receiver, Message msg)
            {
                Receiver.RemoveRemoteDevices(msg.participantId);
                Receiver.StopSending();
            }
        }

        // Tell remote input system that there's a new layout.
        private static class NewLayoutMsg
        {
            [Serializable]
            public struct Data
            {
                public string name;
                public string layoutJson;
                public bool isOverride;
            }

            public static Message? Create(InputRemoting sender, string layoutName)
            {
                // Try to load the layout. Ignore the layout if it couldn't
                // be loaded.
                InputControlLayout layout;
                try
                {
                    layout = sender.m_LocalManager.LoadLayout(new InternedString(layoutName));
                    if (layout == null)
                    {
                        RenderStreaming.Logger.Log(string.Format(
                            "Could not find layout '{0}' meant to be sent through remote connection; this should not happen",
                            layoutName));
                        return null;
                    }
                }
                catch (Exception exception)
                {
                    RenderStreaming.Logger.Log(string.Format(
                        "Could not load layout '{0}'; not sending to remote listeners (exception: {1})", layoutName,
                        exception));
                    return null;
                }

                var data = new Data
                {
                    name = layoutName,
                    layoutJson = layout.ToJson(),
                    isOverride = layout.isOverride
                };

                return new Message
                {
                    type = MessageType.NewLayout,
                    data = SerializeData(data)
                };
            }

            public static void Process(InputRemoting Receiver, Message msg)
            {
                var data = DeserializeData<Data>(msg.data);
                var senderIndex = Receiver.FindOrCreateSenderRecord(msg.participantId);

                var internedLayoutName = new InternedString(data.name);
                Receiver.m_LocalManager.RegisterControlLayout(data.layoutJson, data.name, data.isOverride);
                ArrayHelpers.Append(ref Receiver.m_Senders[senderIndex].layouts, internedLayoutName);
            }
        }

        private static class RemoveLayoutMsg
        {
            public static Message Create(string layoutName)
            {
                var bytes = Encoding.UTF8.GetBytes(layoutName);
                return new Message
                {
                    type = MessageType.RemoveLayout,
                    data = bytes
                };
            }

            public static void Process(InputRemoting Receiver, Message msg)
            {
                ////REVIEW: we probably don't want to do this blindly
                var layoutName = Encoding.UTF8.GetString(msg.data);
                Receiver.m_LocalManager.RemoveLayout(layoutName);
            }
        }

        // Tell remote input system that there's a new device.
        private static class NewDeviceMsg
        {
            [Serializable]
            public struct Data
            {
                public string name;
                public string layout;
                public int deviceId;
                public string[] usages;
                public InputDeviceDescription description;
            }

            public static Message Create(InputDevice device)
            {
                Debug.Assert(!device.remote, "Device being sent to remotes should be a local device, not a remote one");

                var data = new Data
                {
                    name = device.name,
                    layout = device.layout,
                    deviceId = device.deviceId,
                    description = device.description,
                    usages = device.usages.Select(x => x.ToString()).ToArray()
                };

                var json = JsonUtility.ToJson(data);
                var bytes = Encoding.UTF8.GetBytes(json);

                return new Message
                {
                    type = MessageType.NewDevice,
                    data = bytes
                };
            }

            public static void Process(InputRemoting Receiver, Message msg)
            {
                var senderIndex = Receiver.FindOrCreateSenderRecord(msg.participantId);
                var data = DeserializeData<Data>(msg.data);

                // Make sure we haven't already seen the device.
                var devices = Receiver.m_Senders[senderIndex].devices;
                if (devices != null)
                {
                    foreach (var entry in devices)
                        if (entry.remoteId == data.deviceId)
                        {
                            RenderStreaming.Logger.Log(LogType.Error, string.Format(
                                "Already received device with id {0} (layout '{1}', description '{3}) from remote {2}",
                                data.deviceId,
                                data.layout, msg.participantId, data.description));
                            return;
                        }
                }

                // Create device.
                InputDevice device;
                try
                {
                    ////REVIEW: this gives remote devices names the same way that local devices receive them; should we make remote status visible in the name?
                    device = Receiver.m_LocalManager.AddDevice(data.layout, data.name);
                }
                catch (Exception exception)
                {
                    RenderStreaming.Logger.Log(LogType.Error,
                        $"Could not create remote device '{data.description}' with layout '{data.layout}' locally (exception: {exception})");
                    return;
                }
                // todo(kazuki)::Avoid to use reflection
                // device.m_ParticipantId = msg.participantId;
                device.SetParticipantId(msg.participantId);

                // todo(kazuki)::Avoid to use reflection
                // device.m_Description = data.description;
                // device.m_DeviceFlags |= InputDevice.DeviceFlags.Remote;
                device.SetDescription(data.description);

                var deviceFlagsRemote = 1 << 3;
                device.SetDeviceFlags(device.GetDeviceFlags() | deviceFlagsRemote);

                if (data.usages != null)
                    foreach (var usage in data.usages)
                        Receiver.m_LocalManager.AddDeviceUsage(device, usage);

                // Remember it.
                var record = new RemoteInputDevice
                {
                    remoteId = data.deviceId,
                    localId = device.deviceId,
                    description = data.description
                };
                ArrayHelpers.Append(ref Receiver.m_Senders[senderIndex].devices, record);
            }
        }

        // Tell remote system there's new input events.
        private static class NewEventsMsg
        {
            // todo(kazuki):: not found DeviceResetEvent
            //public static unsafe Message CreateResetEvent(InputDevice device, bool isHardReset)
            //{
            //    var resetEvent = DeviceResetEvent.Create(device.deviceId, isHardReset);
            //    return Create((InputEvent*)UnsafeUtility.AddressOf(ref resetEvent), 1);
            //}

            public static unsafe Message CreateStateEvent(InputDevice device)
            {
                using (StateEvent.From(device, out var eventPtr))
                    return Create(eventPtr.data, 1);
            }

            public static unsafe Message Create(InputEvent* events, int eventCount)
            {
                // Find total size of event buffer we need.
                var totalSize = 0u;
                var eventPtr = new InputEventPtr(events);
                for (var i = 0; i < eventCount; ++i, eventPtr = eventPtr.Next())
                {
                    totalSize += eventPtr.sizeInBytes;
                }

                // Copy event data to buffer. Would be nice if we didn't have to do that
                // but unfortunately we need a byte[] and can't just pass the 'events' IntPtr
                // directly.
                var data = new byte[totalSize];
                fixed (byte* dataPtr = data)
                {
                    UnsafeUtility.MemCpy(dataPtr, events, totalSize);
                }

                // Done.
                return new Message
                {
                    type = MessageType.NewEvents,
                    data = data
                };
            }

            public static unsafe void Process(InputRemoting Receiver, Message msg)
            {
                var manager = Receiver.m_LocalManager;

                fixed (byte* dataPtr = msg.data)
                {
                    var dataEndPtr = new IntPtr(dataPtr + msg.data.Length);
                    var eventCount = 0;
                    var eventPtr = new InputEventPtr((InputEvent*)dataPtr);
                    var senderIndex = Receiver.FindOrCreateSenderRecord(msg.participantId);

                    while ((Int64)eventPtr.data < dataEndPtr.ToInt64())
                    {
                        // Patch up device ID to refer to local device and send event.
                        var remoteDeviceId = eventPtr.deviceId;
                        var localDeviceId = Receiver.FindLocalDeviceId(remoteDeviceId, senderIndex);
                        eventPtr.deviceId = localDeviceId;

                        if (localDeviceId != InputDevice.InvalidDeviceId)
                        {
                            ////TODO: add API to send events in bulk rather than one by one
                            manager.QueueEvent(eventPtr);
                        }

                        ++eventCount;
                        eventPtr = eventPtr.Next();
                    }
                }
            }
        }

        private static class ChangeUsageMsg
        {
            [Serializable]
            public struct Data
            {
                public int deviceId;
                public string[] usages;
            }

            public static Message Create(InputDevice device)
            {
                var data = new Data
                {
                    deviceId = device.deviceId,
                    usages = device.usages.Select(x => x.ToString()).ToArray()
                };

                return new Message
                {
                    type = MessageType.ChangeUsages,
                    data = SerializeData(data)
                };
            }

            public static void Process(InputRemoting Receiver, Message msg)
            {
                var senderIndex = Receiver.FindOrCreateSenderRecord(msg.participantId);
                var data = DeserializeData<Data>(msg.data);

                var device = Receiver.TryGetDeviceByRemoteId(data.deviceId, senderIndex);
                if (device != null)
                {
                    foreach (var deviceUsage in device.usages)
                    {
                        if (!data.usages.Contains(deviceUsage))
                            Receiver.m_LocalManager.RemoveDeviceUsage(device, new InternedString(deviceUsage));
                    }

                    if (data.usages.Length == 1)
                        Receiver.m_LocalManager.AddDeviceUsage(device, new InternedString(data.usages[0]));
                    foreach (var dataUsage in data.usages)
                    {
                        var internedDataUsage = new InternedString(dataUsage);
                        if (!device.usages.Contains(internedDataUsage))
                            Receiver.m_LocalManager.AddDeviceUsage(device, new InternedString(dataUsage));
                    }
                }
            }
        }

        private static class RemoveDeviceMsg
        {
            public static Message Create(InputDevice device)
            {
                return new Message
                {
                    type = MessageType.RemoveDevice,
                    data = BitConverter.GetBytes(device.deviceId)
                };
            }

            public static void Process(InputRemoting Receiver, Message msg)
            {
                var senderIndex = Receiver.FindOrCreateSenderRecord(msg.participantId);
                var remoteDeviceId = BitConverter.ToInt32(msg.data, 0);

                var device = Receiver.TryGetDeviceByRemoteId(remoteDeviceId, senderIndex);
                if (device != null)
                    Receiver.m_LocalManager.RemoveDevice(device);
            }
        }

        private static byte[] SerializeData<TData>(TData data)
        {
            var json = JsonUtility.ToJson(data);
            return Encoding.UTF8.GetBytes(json);
        }

        private static TData DeserializeData<TData>(byte[] data)
        {
            var json = Encoding.UTF8.GetString(data);
            return JsonUtility.FromJson<TData>(json);
        }

#if UNITY_EDITOR || DEVELOPMENT_BUILD
        // State we want to take across domain reloads. We can only take some of the
        // state across. Subscriptions will be lost and have to be manually restored.
        [Serializable]
        internal struct SerializedState
        {
            public int senderId;
            public RemoteSender[] senders;

            // We can't take these across domain reloads but we want to take them across
            // InputSystem.Save/Restore.
            [NonSerialized] public Subscriber[] subscribers;
        }

        internal SerializedState SaveState()
        {
            return new SerializedState
            {
                senders = m_Senders,
                subscribers = m_Subscribers
            };
        }

        internal void RestoreState(SerializedState state, IInputManager manager)
        {
            m_LocalManager = manager;
            m_Senders = state.senders;
        }

#endif
    }
}
// #endif
