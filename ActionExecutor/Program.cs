using System;
using System.Net.Http;
using System.IO;
using System.Net;
using Newtonsoft.Json;
using System.Collections.Generic;

namespace ActionExecutor
{
    class Program
    {
        [Serializable]
        public class Action
        {
            public string id;
            public string name;
        }

        [Serializable]
        public class ActionRequest
        {
            public Action action;
            public Dictionary<string, object> args;
        }

        static void Main(string[] args)
        {
            string action = null;
            Dictionary<string, object> cmdArgs = new Dictionary<string, object>();

            foreach (string arg in args)
            {
                if (action == null)
                {
                    action = arg;
                }

                if (arg.Contains("="))
                {
                    string[] parts = arg.Split('=');
                    cmdArgs[parts[0]] = parts[1];
                }
            }
            
            if (action == null)
            {
                Console.WriteLine("Usage: ActionExecutor [action-name]");
                return;
            }

            var httpWebRequest = (HttpWebRequest)WebRequest.Create("http://127.0.0.1:7474/DoAction");
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Method = "POST";

            Console.WriteLine(cmdArgs);
            using (StreamWriter streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                ActionRequest request = new ActionRequest()
                {
                    action = new Action()
                    {
                        name = action
                    },
                    args = cmdArgs
                };

                JsonSerializer serializer = JsonSerializer.Create(new JsonSerializerSettings()
                {
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.Indented,
                });

                serializer.Serialize(streamWriter, request);
            }

            var httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
            using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
            {
                var result = streamReader.ReadToEnd();
                Console.WriteLine($"Status: {httpResponse.StatusCode}");
                if (result != null && result != "")
                {
                    Console.WriteLine($"Response: {result}");
                }
            }
        }
    }
}
