namespace Unity.RenderStreaming.RuntimeTest
{
#if UNITY_EDITOR && !INPUTSYSTEM_1_1_OR_NEWER
    class InputEditorUserSettingsTest
    {
        [SetUp]
        public void SetUp()
        {
            InputEditorUserSettings.Load();
        }

        [TearDown]
        public void TearDown()
        {
            InputEditorUserSettings.Delete();
        }

        [Test]
        public void LockInputToGameView()
        {
            Assert.That(InputEditorUserSettings.lockInputToGameView, Is.False);
            InputEditorUserSettings.lockInputToGameView = true;
            Assert.That(InputEditorUserSettings.lockInputToGameView, Is.True);
        }
    }
#endif
}
