using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Runtime.Caching;

namespace EpiphaneBot.Tests
{
    [TestClass]
    public class SettingsTests : MockCPH
    {
        private SettingsManager settings;

        [TestInitialize]
        public void SetUp()
        {
            settings = new SettingsManager(this, SettingsManager.InMemory);
        }

        [TestCleanup]
        public void TearDown()
        {
        }

        [TestMethod]
        public void TestNestedSettings()
        {
        }
    }
}
