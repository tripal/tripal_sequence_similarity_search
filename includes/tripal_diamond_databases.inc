<?php
// A class of Databases (that already exist and are made available by the admin)

  // Be sure to not confuse Database (for instance a Protein database) with the
 //  actual (Drupal) database where information created by this module is stored.
//   I'll try to be explicit

class DiamondDatabases
{
    
    // Declare the variables for DiamondDatabases
    protected $db_id;       // Database ID
    protected $name;        // Name of the Database
    protected $type;        // Type of database (Protein, Nucleotide)
    protected $version;     // version of the database
    protected $location;    // filepath of the database (on remote if using remote resource)
    
    // Construct the object. Does not handle empty fields that should not be so
    public function __construct($dbInfo = array())
    {
        $this->db_id = $dbInfo['db_id'];
        $this->name = $dbInfo['name'];
        $this->type = $dbInfo['type'];
        $this->version  = array_key_exists('version', $details) ? $details['version'] : ''; //can be null
        $this->location = $dbInfo['location'];
    }
    
    /*
     * @param $filter
     *  The value to filter databases by
     * @param $filterBy
     *  The column to filter the databases by (must be existing column in diamond_db_existing_locations table
     */
    public static function getDatabasesFiltered($filters, $filterBys)
    {
        $queryFilter = "$filterBys[0] LIKE '%$filters[0]%'";
        //$queryFilter = $filterBys[0].' LIKE '.$filters[0];
        for ($i = 1; $i < count($filters); $i++)
        {
            $queryFilter .= "AND $filterBys[$i] LIKE '%$filters[$i]%'";
        }
        $query = "SELECT * FROM diamond_db_existing_locations WHERE $queryFilter ORDER BY name ASC, version DESC";
        $results = db_query($query);
        $databases = array();
        
        //Iterate through the returned database 
        for($i = 0; $i< $results->rowCount();$i++)
        {
          $databases[$i] = $results->fetchAssoc();
        }
        
        return $databases;
    }
    
    public static function getDatabasesSpecific($query)
    {
        $results = db_query($query);
        $databases = array();
        
        //Iterate through the returned database 
        for($i = 0; $i< $results->rowCount();$i++)
        {
          $databases[$i] = $results->fetchAssoc();
        }
        
        return $databases;
    }
    
    public static function getDatabases($type)
    {
        $query = "SELECT * FROM diamond_db_existing_locations WHERE type LIKE '$type' ORDER BY name ASC, version DESC";  
               
        $results = db_query($query);
        $databases = array();
        
        //Iterate through the returned database 
        for($i = 0; $i< $results->rowCount();$i++)
        {
          $databases[$i] = $results->fetchAssoc();
        }
        
        return $databases;
    }
    
    // Return the specified database database info
    public static function getDBInfo($db_id, $field)
    {
        $query = "SELECT $field from diamond_db_existing_locations where db_id = '$db_id'";
        $results = db_query($query);
        return $results->fetchField(0);
    }
    
    // Insert a new database into the... database
    public static function insertDB()
    {
        /* What not to do:
        $query = 'insert into diamond_db_existing_locations (type,name,version,location) values (\'$type\',\'$name\',\'$version\',\'$location\')';
        db_query($query);
         */
        
    }
    
    // Update an existing entry
    public static function updateDB()
    {
        //
    }
    
}


