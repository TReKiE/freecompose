using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Windows;

namespace NewFrontEnd {

    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App: Application {

        public Configuration Configuration { get; set; }

        private string GetConfigurationFileName( ) {
            string CompanyName = string.Empty;
            string ProductName = string.Empty;
            foreach ( object o in Assembly.GetEntryAssembly( ).GetCustomAttributes( true ) ) {
                if ( o is AssemblyCompanyAttribute ) {
                    CompanyName = ( (AssemblyCompanyAttribute) o ).Company;
                } else if ( o is AssemblyProductAttribute ) {
                    ProductName = ( (AssemblyProductAttribute) o ).Product;
                }
            }
            if ( string.IsNullOrWhiteSpace( CompanyName ) ) {
                Debug.Print( "App.App_Startup: No AssemblyCompanyAttribute found on our assembly?" );
                throw new DirectoryNotFoundException( "Unable to obtain company name to construct path to configuration file" );
            }
            if ( string.IsNullOrWhiteSpace( ProductName ) ) {
                Debug.Print( "App.App_Startup: No AssemblyProductAttribute found on our assembly?" );
                throw new DirectoryNotFoundException( "Unable to obtain product name to construct path to configuration file" );
            }

            string ApplicationDataPath = Environment.GetFolderPath( Environment.SpecialFolder.ApplicationData, Environment.SpecialFolderOption.Create );
            if ( string.IsNullOrWhiteSpace( ApplicationDataPath ) ) {
                Debug.Print( "App.App_Startup: can't get ApplicationData special folder" );
                throw new DirectoryNotFoundException( "Unable to obtain path to ApplicationData special folder to construct path to configuration file" );
            }

            try {
                Directory.CreateDirectory( string.Format( @"{0}\{1}", ApplicationDataPath, CompanyName ) );
                Directory.CreateDirectory( string.Format( @"{0}\{1}\{2}", ApplicationDataPath, CompanyName, ProductName ) );
            } catch ( Exception ex ) {
                Debug.Print( "App.GetConfigurationFileName: exception while ensuring product directory exists: {0}\nStack trace:\n{1}", ex.ToString( ), ex.StackTrace );
                throw;
            }

            return string.Format( @"{0}\{1}\{2}\{2}.xml", ApplicationDataPath, CompanyName, ProductName );
        }

        void App_Startup( object sender, StartupEventArgs e ) {
            Debug.Print( "App.App_Startup()" );

            Configuration = new Configuration( );
            try {
                Configuration.Load( GetConfigurationFileName( ) );
            }
            catch ( Exception ex ) {
                // TODO load default configuration from somewhere -- embedded into assembly somehow? 
                Debug.Print( "App.App_Startup: exception trying to load configuration file: {0}\nStack trace:\n{1}", ex.ToString( ), ex.StackTrace );
                throw;
            }
        }

        void App_Exit( object sender, ExitEventArgs e ) {
            Debug.Print( "App.App_Exit()" );
        }

        void App_SessionEnding( object sender, SessionEndingCancelEventArgs e ) {
            Debug.Print( "App.App_SessionEnding()" );
        }

    }

}
