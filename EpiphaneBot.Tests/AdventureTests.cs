using Microsoft.VisualStudio.TestTools.UnitTesting;
using PetaPoco;

namespace EpiphaneBot.Tests
{
    [TestClass]
    public class AdventureTests : MockCPH
    {
        private RPGManager rpg;
        private SettingsManager settings;

        [TestInitialize]
        public void SetUp()
        {
            settings = new SettingsManager(this, SettingsManager.InMemory);

            // https://docs.microsoft.com/en-us/dotnet/standard/data/sqlite/in-memory-databases
            string ConnectionString = "Data Source=RaidTests;Mode=Memory;Cache=Shared";

            var builder = DatabaseConfiguration.Build()
                .UsingProviderName("sqlite")
                .UsingConnectionString(ConnectionString);
            PetaPoco.IDatabase DB = builder.Create();
            User.DeleteTable(DB);

            rpg = new RPGManager(this, settings["RPG"], ConnectionString);

            rpg.BuyCaterium(1, "Epiphane", 5);
            AssertMessage("Epiphane, you now have 10 caterium!");
        }

        [TestCleanup]
        public void TearDown()
        { }

        private void AssertMessage(string message)
        {
            Assert.IsTrue(State.Messages.Count > 0);
            Assert.AreEqual(message, State.Messages.Dequeue());
        }

        private Adventure DefaultRaid()
        {
            Assert.IsTrue(rpg.InitRaid(1, "Epiphane", new[] { "10" }));
            Assert.IsNotNull(rpg.CurrentRaid);
            return rpg.CurrentRaid;
        }

        [TestMethod]
        public void TestInit()
        {
            SettingsManager.Scope RPG = settings.GetScope("RPG");
            Assert.IsTrue(RPG.Has("Raid"));

            SettingsManager.Scope Raid = RPG.GetScope("Raid");
            Assert.IsTrue(Raid.Has("Event"));

            // Confirm an example setting is bound.
            SettingsManager.Scope Event = Raid.GetScope("Event");
            Assert.IsTrue(Event.Has("Stinger"));
            Assert.IsTrue(Event.GetScope("Stinger").Has("Weight"));
        }

        [TestMethod]
        public void TestStartFromMessage()
        {
            rpg.HandleRaidCommand(1, "Epiphane", new[] { "10" });
            Assert.AreEqual(1, State.ActionsRun.Count);
            Assert.AreEqual("Start Raid", State.ActionsRun[0]);
        }

        [TestMethod]
        public void TestStart()
        {
            Adventure raid = DefaultRaid();

            Assert.AreEqual(Adventure.State.NotStarted, raid.state);
        }

        [TestMethod]
        public void TestPrepare()
        {
            Adventure raid = DefaultRaid();

            raid.Prepare();
            Assert.AreEqual(raid.state, Adventure.State.Preparing);

            Assert.AreEqual(1, State.Waits.Count);
            Assert.AreEqual(60000, State.Waits[0]);

            AssertMessage("Epiphane is looking to start a raid! To join, type !raid <amount>");
        }

        [TestMethod]
        public void TestDetails()
        {
            Adventure raid = DefaultRaid();

            Adventure.Details details = raid.GenerateDetails();

            Assert.AreEqual(1, details.Participants.Count);
            Assert.AreEqual(10, details.Participants[0].Investment);
            Assert.AreEqual("Epiphane", details.Participants[0].User.Name);
        }
    }
}
