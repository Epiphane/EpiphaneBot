using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Runtime.Caching;
using System.IO;

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
        public void TestLoadSettings()
        {
            string text = @"{
                ""Setting1"": {
                    ""Sub Setting"": {
                        ""Value"": ""foo""
                    }
                },
                ""Setting2"": {
                    ""Value"": 10
                }
            }";
            settings.ApplySettings(new StringReader(text));

            string foo = settings.GetScope("Setting1").GetScope("Sub Setting").Get<string>("Value");
            Assert.AreEqual("foo", foo);

            Setting<string> fooVar = settings.GetScope("Setting1").GetScope("Sub Setting").At("Value", "not foo");
            Assert.AreEqual("foo", fooVar);
        }

        [TestMethod]
        public void TestLoadSettingsDelayed()
        {
            string foo = settings.GetScope("Setting1").GetScope("Sub Setting").Get<string>("Value");
            Assert.AreEqual("foo", foo);

            Setting<string> fooVar = settings.GetScope("Setting1").GetScope("Sub Setting").At("Value", "not foo");
            Assert.AreEqual("foo", fooVar);

            string text = @"{
                ""Setting1"": {
                    ""Sub Setting"": {
                        ""Value"": ""foo""
                    }
                },
                ""Setting2"": {
                    ""Value"": 10
                }
            }";
            settings.ApplySettings(new StringReader(text));

            foo = settings.GetScope("Setting1").GetScope("Sub Setting").Get<string>("Value");
            Assert.AreEqual("foo", foo);
            Assert.AreEqual("foo", fooVar);
        }
    }
}
