using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Xml.Linq;

namespace NewFrontEnd {

    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App: Application {

        public Configuration Configuration { get; set; }

        void App_Startup( object sender, StartupEventArgs e ) {
            Debug.Print( "App.App_Startup()" );
            Configuration = new Configuration( );
            Configuration.Load( @"C:\Users\jsc\AppData\Roaming\Zive Technology Research\FreeCompose\FreeCompose.xml" );
        }

        void App_Exit( object sender, ExitEventArgs e ) {
            Debug.Print( "App.App_Exit()" );
        }

        void App_SessionEnding( object sender, SessionEndingCancelEventArgs e ) {
            Debug.Print( "App.App_SessionEnding()" );
        }

    }

}
