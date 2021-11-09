using System.IO;

namespace Unity.RenderStreaming.InputSystem
{
    /// <summary>
    /// 
    /// </summary>
    static class MessageSerializer
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public static byte[] Serialize(ref InputRemoting.Message message)
        {
            var stream = new MemoryStream();
            var writer = new BinaryWriter(stream);

            writer.Write(message.participantId);
            writer.Write((int)message.type);
            writer.Write(message.data.Length);
            writer.Write(message.data);

            return stream.ToArray();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="bytes"></param>
        /// <param name="message"></param>
        public static void Deserialize(byte[] bytes, out InputRemoting.Message message)
        {
            var reader = new BinaryReader(new MemoryStream(bytes));

            message = default;
            message.participantId = reader.ReadInt32();
            message.type = (InputRemoting.MessageType)reader.ReadInt32();
            int length = reader.ReadInt32();
            message.data = reader.ReadBytes(length);
        }
    }
}
// #endif
