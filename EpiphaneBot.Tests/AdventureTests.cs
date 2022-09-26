using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PetaPoco;
using static Adventure;

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
            IDatabase DB = builder.Create();
            User.DeleteTable(DB);

            rpg = new RPGManager(this, settings["RPG"], ConnectionString);

            rpg.BuyCaterium(1, "Epiphane", 5);
            AssertMessage("Epiphane, you now have 10 caterium!");
        }

        [TestCleanup]
        public void TearDown()
        {
        }

        private User Epiphane
        {
            get => rpg.GetUser(1, "Epiphane");
        }

        private User AverageZing
        {
            get => rpg.GetUser(2, "AverageZing");
        }

        private void AssertMessage(string message)
        {
            Assert.IsTrue(State.Messages.Count > 0);
            Assert.AreEqual(message, State.Messages.Dequeue());
        }

        private void AssertWait(int milliseconds)
        {
            Assert.IsTrue(State.Waits.Count > 0);
            Assert.AreEqual(milliseconds, State.Waits.Dequeue());

        }

        private Adventure DefaultRaid()
        {
            Assert.IsTrue(rpg.InitRaid(1, "Epiphane", new[] { "10" }));
            Assert.IsNotNull(rpg.CurrentRaid);
            return rpg.CurrentRaid;
        }

        private Adventure TwoPersonRaid()
        {
            Assert.IsTrue(rpg.InitRaid(1, "Epiphane", new[] { "10" }));
            Assert.IsNotNull(rpg.CurrentRaid);

            rpg.BuyCaterium(2, "AverageZing", 5);
            AssertMessage("AverageZing, you now have 10 caterium!");
            Assert.IsTrue(rpg.CurrentRaid.TryJoin(AverageZing, "5", out int amount));
            Assert.AreEqual(5, amount);
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

            AssertWait(60000);
            AssertMessage("Epiphane is looking to start a raid! To join, type !raid <amount>");
        }

        [TestMethod]
        public void TestDetails()
        {
            Adventure raid = DefaultRaid();

            Details details = raid.GenerateDetails();
            Assert.AreEqual(details.Participants, raid.participants);
            Assert.AreEqual(1, details.Participants.Count);
            Assert.AreEqual(10, details.Participants[0].Investment);
            Assert.AreEqual("Epiphane", details.Participants[0].User.Name);
            Assert.AreEqual(Region.NorthernForest, details.Region);
        }

        [TestMethod]
        public void TestDetailsShuffle()
        {
            Adventure raid = TwoPersonRaid();

            Details details = raid.GenerateDetails();
            Assert.AreEqual(details.Participants, raid.participants);
            Assert.AreEqual(2, details.Participants.Count);
            Assert.AreEqual("Epiphane", details.Participants[1].User.Name);
            Assert.AreEqual("AverageZing", details.Participants[0].User.Name);
        }


        [TestMethod]
        public void TestDetailsNoShuffle()
        {
            Adventure raid = TwoPersonRaid();

            Details details = raid.GenerateDetails(false);
            Assert.AreEqual(details.Participants, raid.participants);
            Assert.AreEqual(2, details.Participants.Count);
            Assert.AreEqual("Epiphane", details.Participants[0].User.Name);
            Assert.AreEqual("AverageZing", details.Participants[1].User.Name);
        }

        [TestMethod]
        public void TestInvestmentDetails()
        {
            Adventure raid = TwoPersonRaid();

            Details details = raid.GenerateDetails(false);
            Assert.AreEqual(15, details.TotalInvestment);
            Assert.AreEqual(10, details.MaxInvestment);
            Assert.AreEqual(7.5, details.AverageInvestment);
        }

        [TestMethod]
        public void TestBegin()
        {
            Adventure raid = TwoPersonRaid();

            Details details = raid.GenerateDetails(false);

            raid.Begin(details);
            Assert.AreEqual(raid.state, Adventure.State.Running);
            AssertMessage($"With your 15 caterium, the group purchases some Paleberries for safety...");
            AssertMessage("It's time! Everyone assembles at the HUB and embarks into the Northern Forest...");
            AssertWait(10000);

            // Epiphane has 0 caterium left
            Assert.AreEqual(0, details.Participants[0].User.Caterium);

            // Zing has 5 caterium left
            Assert.AreEqual(5, details.Participants[1].User.Caterium);

            // However, it has not been persisted yet
            Assert.AreEqual(10, Epiphane.Caterium);
            Assert.AreEqual(10, AverageZing.Caterium);
        }
    }
}
