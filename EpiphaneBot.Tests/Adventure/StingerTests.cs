using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PetaPoco;
using static Adventure;

namespace EpiphaneBot.Tests
{
    [TestClass]
    public class StingerTests : MockCPH
    {
        private SettingsManager settings;
        private SettingsManager.Scope scope;
        private readonly Stinger stinger = new Stinger();

        [TestInitialize]
        public void SetUp()
        {
            settings = new SettingsManager(this, SettingsManager.InMemory);
            scope = settings.GetScope("Stinger");

            stinger.Init(this, scope);
        }

        [TestCleanup]
        public void TearDown()
        {
        }

        [TestMethod]
        public void TestForthwind()
        {
        }
    }
}
