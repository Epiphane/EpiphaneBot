using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace EpiphaneBot.Tests
{
    [TestClass]
    public class RaidTests : MockCPH
    {
        private RPGManager rpg;
        private SettingsManager settings;

        [TestInitialize]
        public void SetUp()
        {
            settings = new SettingsManager(this, SettingsManager.InMemory);
        
            // https://docs.microsoft.com/en-us/dotnet/standard/data/sqlite/in-memory-databases
            string ConnectionString = "Data Source=RaidTests;Mode=Memory;Cache=Shared";
            rpg = new RPGManager(this, settings["Raid"], ConnectionString);
        }

        [TestCleanup]
        public void TearDown()
        { }

        [TestMethod]
        public void TestStartRaidFromMessage()
        {
            rpg.HandleRaidCommand(1, "Epiphane", new[] { "10" });
            Assert.AreEqual(State.ActionsRun.Count, 1);
            Assert.AreEqual(State.ActionsRun[0], "Start Raid");
        }

        [TestMethod]
        public void TestStartRaid()
        {
            rpg.StartRaid(1, "Epiphane", new[] { "10" });
            Assert.IsNotNull(rpg.CurrentRaid);

            Adventure raid = rpg.CurrentRaid;
        }
    }
}
