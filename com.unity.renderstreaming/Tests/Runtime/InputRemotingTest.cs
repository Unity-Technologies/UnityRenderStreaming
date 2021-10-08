using System;
using System.Linq;
using System.Collections;
using System.Collections.Generic;
using NUnit.Framework;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Users;
using Unity.RenderStreaming.RuntimeTest.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.TestTools;
using Assert = NUnit.Framework.Assert;

namespace Unity.RenderStreaming.RuntimeTest
{
    class MessageSerializerTest
    {
        [Test]
        public void Serialize()
        {
            InputRemoting.Message message1 = new InputRemoting.Message
            {
                participantId = 1,
                type = InputRemoting.MessageType.NewEvents,
                data = new byte[] {1, 2, 3, 4, 5},
            };

            var bytes = MessageSerializer.Serialize(ref message1);

            Assert.That(bytes, Is.Not.Null);
            Assert.That(bytes, Has.Length.GreaterThan(0));

            MessageSerializer.Deserialize(bytes, out var message2);
            Assert.That(message2.participantId, Is.EqualTo(message1.participantId));
            Assert.That(message2.type, Is.EqualTo(message1.type));
            Assert.That(message2.data, Is.EqualTo(message1.data));
        }
    }

    class InputDeviceExtensionTest
    {
        [Test]
        public void SetDescription()
        {
            var device = InputSystem.AddDevice<Mouse>();
            InputDeviceDescription empty;
            InputDeviceDescription origin = device.description;
            device.SetDescription(empty);
            Assert.That(device.description, Is.EqualTo(empty));
            device.SetDescription(origin);
            Assert.That(device.description, Is.EqualTo(origin));
        }
        [Test]
        public void SetParticipantId()
        {
            var device = InputSystem.AddDevice<Mouse>();
            int participantId = 0;
            int origin = device.GetParticipantId();
            device.SetParticipantId(participantId);
            Assert.That(device.GetParticipantId(), Is.EqualTo(participantId));
            device.SetParticipantId(origin);
            Assert.That(device.GetParticipantId(), Is.EqualTo(origin));
        }
        [Test]
        public void SetDeviceFlags()
        {
            var device = InputSystem.AddDevice<Mouse>();
            int deviceFlags = 0;
            int origin = device.GetDeviceFlags();
            device.SetDeviceFlags(deviceFlags);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(deviceFlags));
            device.SetDeviceFlags(origin);
            Assert.That(device.GetDeviceFlags(), Is.EqualTo(origin));
        }
    }

    /// todo(kazuki):workaround
    class InputRemotingTest
    {
        class MyMonoBehaviourTest : MonoBehaviour, IMonoBehaviourTest
        {
            public bool IsTestFinished
            {
                get { return true; }
            }
        }

        private MonoBehaviourTest<MyMonoBehaviourTest> _test;
        private RenderStreamingInternal _target1, _target2;
        private RTCDataChannel _channel1, _channel2;
        private string connectionId = "12345";
        const float ResendOfferInterval = 1.0f;

        private RenderStreamingDependencies CreateDependencies()
        {
            return new RenderStreamingDependencies
            {
                signaling = new MockSignaling(),
                config = new RTCConfiguration
                {
                    iceServers = new[] { new RTCIceServer { urls = new[] { "stun:stun.l.google.com:19302" } } },
                },
                encoderType = EncoderType.Software,
                startCoroutine = _test.component.StartCoroutine,
                resentOfferInterval = ResendOfferInterval,
            };
        }

        [UnitySetUp]
        public IEnumerator UnitySetUp()
        {
            MockSignaling.Reset(true);
            _test = new MonoBehaviourTest<MyMonoBehaviourTest>();

            var dependencies1 = CreateDependencies();
            var dependencies2 = CreateDependencies();
            _target1 = new RenderStreamingInternal(ref dependencies1);
            _target2 = new RenderStreamingInternal(ref dependencies2);

            bool isStarted1 = false;
            bool isStarted2 = false;
            _target1.onStart += () => { isStarted1 = true; };
            _target2.onStart += () => { isStarted2 = true; };
            yield return new WaitUntil(() => isStarted1 && isStarted2);

            bool isCreatedConnection1 = false;
            bool isCreatedConnection2 = false;
            _target1.onCreatedConnection += _ => { isCreatedConnection1 = true; };
            _target2.onCreatedConnection += _ => { isCreatedConnection2 = true; };

            // _target1 is Receiver in private mode
            _target1.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection1);

            // _target2 is sender in private mode
            _target2.CreateConnection(connectionId);
            yield return new WaitUntil(() => isCreatedConnection2);

            _target1.onAddChannel += (_, channel) => { _channel1 = channel; };

            // send offer automatically after creating channel
            _channel2 = _target2.CreateChannel(connectionId, "_test");

            bool isGotOffer1 = false;
            bool isGotAnswer2 = false;
            _target1.onGotOffer += (_, sdp) => { isGotOffer1 = true; };
            _target2.onGotAnswer += (_, sdp) => { isGotAnswer2 = true; };

            // each peer are not stable, signaling process not complete.
            yield return new WaitUntil(() => isGotOffer1);

            _target1.SendAnswer(connectionId);
            yield return new WaitUntil(() => isGotAnswer2);
            Assert.That(isGotAnswer2, Is.True);

            // If target1 processes resent　Offer from target2, target1 is not stable.
            Assert.That(_target2.IsStable(connectionId), Is.True);
        }

        [UnityTearDown]
        public IEnumerator UnityTearDown()
        {
            _target1.DeleteConnection(connectionId);
            _target2.DeleteConnection(connectionId);

            bool isDeletedConnection1 = false;
            bool isDeletedConnection2 = false;
            _target1.onDeletedConnection += _ => { isDeletedConnection1 = true; };
            _target2.onDeletedConnection += _ => { isDeletedConnection2 = true; };
            yield return new WaitUntil(() => isDeletedConnection1 && isDeletedConnection2);

            _channel1.Dispose();
            _channel2.Dispose();

            _test.component.StopAllCoroutines();
            _target1.Dispose();
            _target2.Dispose();
            UnityEngine.Object.DestroyImmediate(_test.gameObject);
        }

        [Test]
        public void DoNothing()
        {
        }


        [Test]
        public void Sender()
        {
            var sender = new Sender();
            Assert.That(sender.layouts, Is.Not.Empty);
            Assert.That(sender.devices, Is.Not.Empty);
            var senderInput = new InputRemoting(sender);
            var senderDisposer = senderInput.Subscribe(new Observer(_channel1));
            senderInput.StartSending();
            senderInput.StopSending();
            senderDisposer.Dispose();
        }

        [Test]
        public void Receiver()
        {
            var receiver = new Receiver(_channel1);
            Assert.That(receiver.remoteDevices, Is.Empty);
            Assert.That(receiver.remoteLayouts, Is.Empty);
            var receiverInput = new InputRemoting(receiver);
            var receiverDisposer = receiverInput.Subscribe(receiverInput);
            receiverInput.StartSending();
            receiverInput.StopSending();
            receiverDisposer.Dispose();
        }

        /// todo(kazuki): This test is failed for timeout on macOS 
        [UnityTest, Timeout(3000)]
        [UnityPlatform(exclude = new[] { RuntimePlatform.OSXPlayer })]
        public IEnumerator AddDevice()
        {
            var sender = new Sender();
            var senderInput = new InputRemoting(sender);
            var senderSubscriberDisposer = senderInput.Subscribe(new Observer(_channel1));

            var receiver = new Receiver(_channel2);
            var receiverInput = new InputRemoting(receiver);
            var receiverSubscriberDisposer = receiverInput.Subscribe(receiverInput);

            InputDevice device = null;
            InputDeviceChange change = default;
            receiver.onDeviceChange += (_device, _change) => {
                device = _device;
                change = _change;
            };
            string layoutName = null;
            InputControlLayoutChange layoutChange = default;
            receiver.onLayoutChange += (_name, _change) => {
                layoutName = _name;
                layoutChange = _change;
            };

            receiverInput.StartSending();
            senderInput.StartSending();

            yield return new WaitUntil(() => device != null);
            yield return new WaitUntil(() => layoutName != null);

            Assert.That(device, Is.Not.Null);
            Assert.That(change, Is.EqualTo(InputDeviceChange.Added));
            Assert.That(layoutName, Is.Not.Null);
            Assert.That(layoutChange, Is.EqualTo(InputControlLayoutChange.Added));

            Assert.That(receiver.remoteLayouts, Is.Not.Empty);
            Assert.That(receiver.remoteDevices, Is.Not.Empty);
            Assert.That(receiver.remoteDevices, Has.All.Matches<InputDevice>(d => d.remote));

            senderInput.StopSending();
            receiverInput.StopSending();

            senderSubscriberDisposer.Dispose();
            receiverSubscriberDisposer.Dispose();
            sender.Dispose();
            receiver.Dispose();
        }
    }

    class MultiplayInputManager
    {
        private Action<InputControl, InputEventPtr> m_UnpairedDeviceUsedDelegate;

        // Key: InputUser.id, value
        private Dictionary<InputDevice, uint> devices = new Dictionary<InputDevice, uint>();

        static MultiplayInputManager()
        {
            instance = new MultiplayInputManager();
        }

        private MultiplayInputManager()
        {
        }

        public static MultiplayInputManager instance { get; private set; }

        ~MultiplayInputManager()
        {
            DisableJoining();
        }

        public void EnableJoining()
        {
            if (m_UnpairedDeviceUsedDelegate == null)
                m_UnpairedDeviceUsedDelegate = OnUnpairedDeviceUsed;
            InputUser.onUnpairedDeviceUsed += m_UnpairedDeviceUsedDelegate;
            ++InputUser.listenForUnpairedDeviceActivity;
        }

        public void DisableJoining()
        {
            InputUser.onUnpairedDeviceUsed -= m_UnpairedDeviceUsedDelegate;
            --InputUser.listenForUnpairedDeviceActivity;
        }

        public void AddDevice(InputDevice device, uint id)
        {
            devices.Add(device, id);
        }

        public void RemoveDevice(InputDevice device, uint id)
        {
            if (devices[device] != id)
                throw new InvalidOperationException($"user {id} is not owner of {device}.");
            devices.Remove(device);
        }

        private void OnUnpairedDeviceUsed(InputControl control, InputEventPtr eventPtr)
        {
            //Debug.Log("OnUnpairedDeviceUsed");
            uint id = devices[control.device];
            //var playerInput = PlayerInput.all.FirstOrDefault(_ => _.user.id == id);
            //var result = playerInput.SwitchCurrentControlScheme(control.device);

            InputUser user = InputUser.all.First(_ => _.id == id);
            InputUser.PerformPairingWithDevice(control.device, user);
            //user.ActivateControlScheme(controlScheme);
        }
    }

    static class PlayerInputExtension
    {
        public static void AddDevice(this PlayerInput owner, InputDevice device)
        {
            MultiplayInputManager.instance.AddDevice(device, owner.user.id);
        }
        public static void RemoveDevice(this PlayerInput owner, InputDevice device)
        {
            MultiplayInputManager.instance.RemoveDevice(device, owner.user.id);
        }
    }

    class PlayerInputTest : InputTestFixture
    {
        [Test]
        public void Test()
        {
            var actions = ScriptableObject.CreateInstance<InputActionAsset>();
            var playActions = new InputActionMap("play");
            var fireAction = playActions.AddAction("fire");
            fireAction.AddBinding("<Gamepad>/<Button>"); // Match multiple controls in single binding to make sure that isn't throwing PlayerInput off.
            fireAction.AddBinding("<Keyboard>/space");
            actions.AddActionMap(playActions);

            var go = new GameObject();
            go.SetActive(false);
            var playerInput1 = go.AddComponent<PlayerInput>();
            playerInput1.actions = actions;
            go.SetActive(true);

            Assert.That(playerInput1.devices, Is.Empty);
        }

        private void OnUnpairedDeviceUsed(InputControl control, InputEventPtr eventPtr)
        {
        }

        [Test]
        public void PlayerInput()
        {
            //var gamepad1 = InputSystem.AddDevice<Gamepad>();
            //var gamepad2 = InputSystem.AddDevice<Gamepad>();
            //var gamepad3 = InputSystem.AddDevice<Gamepad>();
            //var gamepad4 = InputSystem.AddDevice<Gamepad>();
            //var scheme1 = new InputControlScheme("test1").WithRequiredDevice(gamepad1.name);
            //var scheme2 = new InputControlScheme("test2").WithRequiredDevice(gamepad2.name);
            //var scheme3 = new InputControlScheme("test3").WithRequiredDevice(gamepad3.name);
            //var scheme4 = new InputControlScheme("test4").WithRequiredDevice(gamepad4.name);

            MultiplayInputManager manager = MultiplayInputManager.instance;
            manager.EnableJoining();

            var actions1 = ScriptableObject.CreateInstance<InputActionAsset>();
            //actions1.AddControlScheme(scheme1);
            //actions1.AddControlScheme(scheme2);
            var actions2 = ScriptableObject.CreateInstance<InputActionAsset>();
            //actions2.AddControlScheme(scheme3);
            //actions2.AddControlScheme(scheme4);

            var go1 = new GameObject();
            var playerInput1 = go1.AddComponent<PlayerInput>();
            playerInput1.neverAutoSwitchControlSchemes = true;
            playerInput1.actions = actions1;
            var go2 = new GameObject();
            var playerInput2 = go2.AddComponent<PlayerInput>();
            playerInput2.neverAutoSwitchControlSchemes = true;
            playerInput2.actions = actions2;

            var gamepad1 = InputSystem.AddDevice<Gamepad>();
            var gamepad2 = InputSystem.AddDevice<Gamepad>();
            var gamepad3 = InputSystem.AddDevice<Gamepad>();
            var gamepad4 = InputSystem.AddDevice<Gamepad>();

            //playerInput1.AddDevice(gamepad1);
            //playerInput1.AddDevice(gamepad2);
            //playerInput2.AddDevice(gamepad3);
            //playerInput2.AddDevice(gamepad4);

            Assert.That(playerInput1.user.valid, Is.False);
            Assert.That(playerInput2.user.valid, Is.False);

            //Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test1"));
            //Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            //Press(gamepad1.buttonSouth);
            //Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test1"));
            //Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            //Press(gamepad2.buttonSouth);
            //Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test2"));
            //Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            //Press(gamepad4.buttonSouth);
            //Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test2"));
            //Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test4"));

            //manager.DisableJoining();
        }


        [Test]
        public void ControlScheme()
        {
            var gamepad1 = InputSystem.AddDevice<Gamepad>();
            var gamepad2 = InputSystem.AddDevice<Gamepad>();
            var gamepad3 = InputSystem.AddDevice<Gamepad>();
            var gamepad4 = InputSystem.AddDevice<Gamepad>();
            var scheme1 = new InputControlScheme("test1").WithRequiredDevice(gamepad1.name);
            var scheme2 = new InputControlScheme("test2").WithRequiredDevice(gamepad2.name);
            var scheme3 = new InputControlScheme("test3").WithRequiredDevice(gamepad3.name);
            var scheme4 = new InputControlScheme("test4").WithRequiredDevice(gamepad4.name);

            MultiplayInputManager manager = MultiplayInputManager.instance;
            manager.EnableJoining();

            var actions1 = ScriptableObject.CreateInstance<InputActionAsset>();
            actions1.AddControlScheme(scheme1);
            actions1.AddControlScheme(scheme2);
            var actions2 = ScriptableObject.CreateInstance<InputActionAsset>();
            actions2.AddControlScheme(scheme3);
            actions2.AddControlScheme(scheme4);

            var go1 = new GameObject();
            var playerInput1 = go1.AddComponent<PlayerInput>();
            playerInput1.actions = actions1;
            var go2 = new GameObject();
            var playerInput2 = go2.AddComponent<PlayerInput>();
            playerInput2.actions = actions2;

            //playerInput1.AddDevice(gamepad1);
            //playerInput1.AddDevice(gamepad2);
            //playerInput2.AddDevice(gamepad3);
            //playerInput2.AddDevice(gamepad4);

            Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test1"));
            Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            Press(gamepad1.buttonSouth);
            Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test1"));
            Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            Press(gamepad2.buttonSouth);
            Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test2"));
            Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test3"));

            Press(gamepad4.buttonSouth);
            Assert.That(playerInput1.currentControlScheme, Is.EqualTo("test2"));
            Assert.That(playerInput2.currentControlScheme, Is.EqualTo("test4"));

            manager.DisableJoining();
        }

        [Test]
        public void InputUserTest()
        {
            var gamepad1 = InputSystem.AddDevice<Gamepad>();
            var gamepad2 = InputSystem.AddDevice<Gamepad>();
            var gamepad3 = InputSystem.AddDevice<Gamepad>();
            var gamepad4 = InputSystem.AddDevice<Gamepad>();
            var scheme1 = new InputControlScheme("test1").WithRequiredDevice(gamepad1.name);
            var scheme2 = new InputControlScheme("test2").WithRequiredDevice(gamepad2.name);
            var scheme3 = new InputControlScheme("test3").WithRequiredDevice(gamepad3.name);
            var scheme4 = new InputControlScheme("test4").WithRequiredDevice(gamepad4.name);

            MultiplayInputManager manager = MultiplayInputManager.instance;
            manager.EnableJoining();

            var actions1 = ScriptableObject.CreateInstance<InputActionAsset>();
            actions1.AddControlScheme(scheme1);
            actions1.AddControlScheme(scheme2);
            var actions2 = ScriptableObject.CreateInstance<InputActionAsset>();
            actions2.AddControlScheme(scheme3);
            actions2.AddControlScheme(scheme4);

            InputUser user1 = InputUser.CreateUserWithoutPairedDevices();
            InputUser user2 = InputUser.CreateUserWithoutPairedDevices();
            user1.AssociateActionsWithUser(actions1);
            user2.AssociateActionsWithUser(actions2);

            InputUser.PerformPairingWithDevice(gamepad1, user1);
            InputUser.PerformPairingWithDevice(gamepad2, user1);
            InputUser.PerformPairingWithDevice(gamepad3, user2);
            InputUser.PerformPairingWithDevice(gamepad4, user2);

            //manager.AddDevice(gamepad1, user1.id);
            //manager.AddDevice(gamepad2, user1.id);
            //manager.AddDevice(gamepad3, user2.id);
            //manager.AddDevice(gamepad4, user2.id);
            //playerInput1.AddDevice(gamepad1);
            //playerInput1.AddDevice(gamepad2);
            //playerInput1.UpdateAssociatedInputActions();
            //playerInput2.AddDevice(gamepad3);
            //playerInput2.AddDevice(gamepad4);

            //PlayerInput.
            //Debug.Log(playerInput1.user.pairedDevices);


            Press(gamepad1.buttonSouth);
            //Assert.That(user1.pairedDevices, Is.EquivalentTo(new[] { gamepad1 }));
            //Assert.That(user2.pairedDevices, Is.EquivalentTo(new InputDevice[] {}));

            Press(gamepad2.buttonSouth);
            //Assert.That(user1.pairedDevices, Is.EquivalentTo(new[] { gamepad1 }));
            //Assert.That(user2.pairedDevices, Is.EquivalentTo(new InputDevice[] { }));

            //
            //manager.DisableJoining();
        }
    }

}
