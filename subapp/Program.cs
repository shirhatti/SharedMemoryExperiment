using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace subapp
{
    class Program
    {
        static async Task<int> Main(string[] args)
        {
            var socketPath = Path.Combine(Directory.GetCurrentDirectory(), "temp.sock");
            var endPoint = new UnixDomainSocketEndPoint(socketPath);
            using (Socket sock = new Socket(AddressFamily.Unix, SocketType.Stream, ProtocolType.Unspecified))
            {  
                await sock.ConnectAsync(endPoint);
                byte[] msg = Encoding.UTF8.GetBytes("This is a test");
                sock.Send(msg);
            }
            return 0;
        }
    }
}
