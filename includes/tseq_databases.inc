<?php
// A class of Databases (that already exist and are made available by the admin)

  // Be sure to not confuse Database (for instance a Protein database) with the
 //  actual (Drupal) database where information created by this module is stored.
//   I'll try to be explicit

class TSeqDatabases
{
    
    // Declare the variables for TSeqDatabases
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
     *  The column to filter the databases by (must be existing column in tseq_existing_locations table
     */
    public static function getDatabasesFiltered($filters, $filterBys)
    {
        $queryFilter = "$filterBys[0] LIKE '%$filters[0]%'";
        //$queryFilter = $filterBys[0].' LIKE '.$filters[0];
        for ($i = 1; $i < count($filters); $i++)
        {
            $queryFilter .= "AND $filterBys[$i] LIKE '%$filters[$i]%'";
        }
        $query = "SELECT * FROM tseq_db_existing_locations WHERE $queryFilter ORDER BY name ASC, version DESC";
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
    
    public static function getDatabasesByTypeCategory($type, $category)
    {
        $query = "SELECT * FROM tseq_db_existing_locations WHERE type LIKE '$type' and category LIKE '$category' ORDER BY name ASC, version DESC";  

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
        $query = "SELECT * FROM tseq_db_existing_locations WHERE type LIKE '$type' ORDER BY name ASC, version DESC";  

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
        $query = "SELECT $field from tseq_db_existing_locations where db_id = '$db_id'";
        $results = db_query($query);
        return $results->fetchField(0);
    }
    
    // Insert a new database into the... database
    public static function insertDB()
    {
        /* What not to do:
        $query = 'insert into tseq_existing_locations (type,name,version,location) values (\'$type\',\'$name\',\'$version\',\'$location\')';
        db_query($query);
         */
        
    }

    /**
     * Check if the database file exists and is readable.
     * Keep this as a static function to reduce time and memory usage - we don't need to load
     *  an instance of every database object just to check a file (which will already introduce
     *  a time delay to to IO wait)
     *
     * @param int $db_id
     * @return integer
     *  0 - exists, readable
     *  1 - exists, unreadable
     *  2 - doesn't exist
     *  3 - unknown database ID
     */
    public static function db_check($db_id, $location="")
    {
        if($location == ""){
            $location = TSeqDatabases::getDBInfo($db_id,'location');
        }
        if (!$location)
        {
            return 3;
        }
        // File ID makes sense, check if the file exists. This also checks for dirs
        if (file_exists($location))
        {
            // File exists, but is it readable?
            // Yes.
            if (is_readable($location))
            {
                return 0;
            }
            // No.
            else
            {
                return 1;
            }
        }
        // Indicated file may not exist verbatim, lets fuzzy search. It could also
        // not be readable by the webserver.
        else
        {
            // Specific File might not exist, but we can try a fuzzy search
            // e.g /some/path/to/file passes the "file exists" test for a directory:
            //      file.type1
            //      file.type2
            //      file.type3
            // Lets get a list of files that might match, see if they fit the profile
            //  e.g. blast databases that are comprised of the following files
            //  - nhr - header
            //  - nin - index
            //  - nsq - sequence
            //  - nog, nsd, nsi
            
            // Get the directory contents
            // Get the parent path and the file name (without blast extensions)
            $parent_path = implode(explode('/',$location,-1),'/');
            $file_name_array = explode('/',$location);
            $file_name = $file_name_array[count($file_name_array) - 1];
            if (is_dir($parent_path))
            {
                // Test if the directory is even readable by the webserver
                if (!is_readable($parent_path))
                {
                    return 1;
                }
                $dir_contents = scandir($parent_path);
                $matches = 0;
                foreach($dir_contents as $dir_file)
                {
                    // Does a file look like it might match?
                    // @TODO make this better detect actual blast index extensions
                    $file_reg = '/\b(\w*'.$file_name.'\w*)\b/';
                    if(preg_match($file_reg,$dir_file))
                    {
                        // @TODO make sure the file is readable
                        $matches += 1;
                    }
                }
                if($matches == 0)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 2;
            }
            /*if ()
            {

            }
            // File really doesn't exist
            else
            {
                return 2;
            }
            */
        }
    }
    
    // Update an existing entry
    public static function updateDB()
    {
        //
    }
    
}


