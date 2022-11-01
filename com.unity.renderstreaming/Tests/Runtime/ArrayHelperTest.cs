using NUnit.Framework;

namespace Unity.RenderStreaming.RuntimeTest
{

    class ArrayHelpersTest
    {
        [Test]
        public void LengthSafe()
        {
            var array = new int[] { 1, 2, 3 };
            Assert.That(ArrayHelpers.LengthSafe(array), Is.EqualTo(3));

            array = null;
            Assert.That(ArrayHelpers.LengthSafe(array), Is.EqualTo(0));
        }

        [Test]
        public void Append()
        {
            var array = new int[] { 1, 2, 3 };
            ArrayHelpers.Append(ref array, 4);
            Assert.That(array, Is.EqualTo(new int[] { 1, 2, 3, 4 }));

            array = null;
            ArrayHelpers.Append(ref array, 1);
            Assert.That(array, Is.EqualTo(new int[] { 1 }));

        }

        [Test]
        public void AppendArray()
        {
            var array = new int[] { 1, 2, 3 };
            var array2 = new int[] { 4, 5 };
            ArrayHelpers.Append(ref array, array2);
            Assert.That(array, Is.EqualTo(new int[] { 1, 2, 3, 4, 5 }));

            array = null;
            ArrayHelpers.Append(ref array, array2);
            Assert.That(array, Is.EqualTo(new int[] { 4, 5 }));
        }

        [Test]
        public void IndexOf()
        {
            var array = new int[] { 1, 2, 3 };
            Assert.That(ArrayHelpers.IndexOf(array, 2), Is.EqualTo(1));
            Assert.That(ArrayHelpers.IndexOf(array, 4), Is.EqualTo(-1));

            array = null;
            Assert.That(ArrayHelpers.IndexOf(array, 2), Is.EqualTo(-1));
        }

        [Test]
        public void Erase()
        {
            var array = new int[] { 1, 2, 3 };
            Assert.That(ArrayHelpers.Erase(ref array, 2), Is.True);
            Assert.That(array, Is.EqualTo(new int[] { 1, 3 }));

            Assert.That(ArrayHelpers.Erase(ref array, 2), Is.False);
            Assert.That(array, Is.EqualTo(new int[] { 1, 3 }));

            array = null;
            Assert.That(ArrayHelpers.Erase(ref array, 2), Is.False);
            Assert.That(array, Is.Null);
        }


        [Test]
        public void EraseAt()
        {
            var array = new int[] { 1, 2, 3 };
            ArrayHelpers.EraseAt(ref array, 1);
            Assert.That(array, Is.EqualTo(new int[] { 1, 3 }));

            array = new int[] { 1 };
            ArrayHelpers.EraseAt(ref array, 0);
            Assert.That(array, Is.Null);
        }

        [Test]
        public void PutAtIfNotSet()
        {
            var array = new int[] { 1, 2, 3 };
            ArrayHelpers.PutAtIfNotSet(ref array, 3, () => { return 4; });
            Assert.That(array, Is.EqualTo(new int[] { 1, 2, 3, 4 }));
        }
    }
}
