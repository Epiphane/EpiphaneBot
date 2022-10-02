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
            SettingsManager.Scope Setting1 = settings.GetScope("Setting1");
            SettingsManager.Scope SubSetting = Setting1.GetScope("Sub Setting");

            string foo = SubSetting.Get<string>("Value", "foo default");
            Assert.AreEqual("foo default", foo);

            Setting<string> fooVar = SubSetting.At("Value", "not foo");
            Assert.AreEqual("not foo", fooVar);

            int bar = Setting1.Get("bar", 12);
            Assert.AreEqual(12, bar);

            Setting<int> barVar = Setting1.At("bar", 11);
            Assert.AreEqual(11, barVar);

            string text = @"{
                ""Setting1"": {
                    ""Sub Setting"": {
                        ""Value"": ""foo""
                    },
                    ""bar"": 10
                }
            }";
            settings.ApplySettings(new StringReader(text));

            foo = settings.GetScope("Setting1").GetScope("Sub Setting").Get<string>("Value");
            Assert.AreEqual("foo", foo);
            Assert.AreEqual("foo", fooVar);

            bar = Setting1.Get("bar", 12);
            Assert.AreEqual(10, bar);
            Assert.AreEqual(10, barVar);
        }
    }
}
