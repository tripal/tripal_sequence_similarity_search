<?php

/**
 * Implements hook_page_build()
 *   Builds a page for admin to view and interact with the database list.
 *   This is the main page that gets shown in the Admin/Config menu.
 */
function tseq_config_dblist_page_build()
{
    // Build the breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>');
    $breadcrumb[] = l('Administration', 'admin');
    $breadcrumb[] = l('Tripal','admin/tripal');
    $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
    $breadcrumb[] = l('TSeq', 'admin/tripal/extension/tseq/config');
    $breadcrumb[] = l('Available Databases','admin/tripal/extension/tseq/config/dblist');    
    drupal_set_breadcrumb($breadcrumb);
    
    // Generate link to add another database
    $contents['addLink'] = array(
        '#markup'   => l('Add an existing database','./admin/tripal/extension/tseq/add_db'),
        '#weight'   => '1',
    );
    
    // Build the table variables
    $database_header = array(
        array('data' => 'Name',     'field' => 'name',        'sort' => 'asc'),
        array('data' => 'Version',  'field' => 'version'),
        array('data' => 'Type',     'field' => 'type'),
        array('data' => 'Category', 'field' => 'category'),
        array('data' => 'Location (Remote)',    'field' => 'location'),
        array('data' => 'Count',          'field' => 'count'),
        array('data' => 'Web?',     'field' => 'web_location'),
        array('data' => 'Actions'),            
    );
    
    // Generate the database query (with sorting and pagination)
    $select = db_select('tseq_db_existing_locations','db')
            ->extend('PagerDefault')
            ->extend('TableSort');
    
    $select->fields('db',array('db_id','name','version','type','category','location','count','web_location'))
            ->limit(15)
            ->orderByHeader($database_header);
    $databases = $select->execute();
    $database_list = array();
    foreach($databases as $key => $database)
    {
        $edit = l('Edit','admin/tripal/extension/tseq/edit_db/'.$database->db_id);
        $delete = l('Delete','admin/tripal/extension/tseq/delete_db/'.$database->db_id);
        if ($database->web_location)
        {
            $web_available = "<abbr title=\"".$database->web_location."\">Yes</abbr>";
        }
        else
        {
            $web_available = "<abbr title=\"Add one by clicking 'Edit' to the right\">No</abbr>";
        }

        $db_check = TSeqDatabases::db_check($database->db_id);
        switch ($db_check)
        {
            case 0:
                $db_error = '';
                break;
            case 1:
                $db_error = 'db_unreadable';
                break;
            case 2:
                $db_error = 'db_missing';
                break;
            default:
                $db_error = '';
        }

        $database_list[] = array(
            $database->name,
            $database->version,
            $database->type,
            $database->category,
            array(
                'data' => $database->location,
                'class' => array(
                    $db_error
                )
            ),
            $database->count,
            $web_available,
            $edit.' | '.$delete,
        );
    }
    
    $output = theme('table', array('header' => $database_header, 'rows' => $database_list));
    
    $output .= theme('pager');
    
    $contents['database_list'] = array(
        '#markup'   => $output,
        '#weight'   => '4',
    );
    
    return $contents;
}

/**
 * Implements hook_form()
 *   Generates a form for admins to add a database (target sequence) file
 *   to the list.
 */
function tseq_database_add_form($form, &$form_state)
{
    // Build the breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>');
    $breadcrumb[] = l('Administration', 'admin');
    $breadcrumb[] = l('Tripal','admin/tripal');
    $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
    $breadcrumb[] = l('TSeq', 'admin/tripal/extension/tseq/config');
    $breadcrumb[] = l('Add Target Database','admin/tripal/extension/tseq/config/add_db');    
    drupal_set_breadcrumb($breadcrumb);
    
    /*
     * Getting categories from the database
     */
    $query = 'select * from tseq_categories';
    $results = db_query($query);
    
    $categories = array();
        
    //Iterate through the returned database 
    for($i = 0; $i< $results->rowCount();$i++)
    {
      $categories[$i] = $results->fetchAssoc();
    }
    
    //Generate options for select form element
    foreach($categories as $category)
    {
        $categoriesToShow[$category['category_title']] = $category['category_title'];
    }
    
    $form['Name'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Name',
        '#size'         => '50',
        '#description'  => 'The name of the target database',
        '#required'     => true,
    );
    
    $form['Type'] = array(
        '#type'         => 'select',
        '#title'        => 'Type',
        '#description'  => 'The type of target database being added',
        '#options'      => array(
            'Protein'       => t('Protein'),
            'Genome'    => t('Genome'),
            'Gene'      => t('Gene'),
        ),
        '#required'     => true,
    );
    
    $form['Category'] = array(
        '#type'         => 'select',
        '#title'        => 'Category', 
        '#options'      => $categoriesToShow,
        '#description'  => 'The category of the database',
        '#required'     => true,
    );
    
    $form['Version'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Version',
        '#size'         => '7',
        '#description'  => 'The version of the added database',
        '#required'     => true,
    );
    
    $form['Location'] = array(
        '#type'         => 'textfield',
        '#title'        => 'File location',
        '#size'         => '120',
        '#description'  => 'The path to the indexed database on disk (accessible on the remote server)',
        '#required'     => true,
    );
    
    $form['WebLocation'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Web Location (Optional)',
        '#size'         => '120',
        '#description'  => 'If the original sequence (non-indexed) is publicly available for download, set the URL here (FTP, HTTP)',
        '#required'     => false,
    );
    
    $form['Count'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Genes, Proteins, or Scaffolds (Optional)',
        '#size'         => '10',
        '#description'  => 'How many genes, proteins, or scaffolds the sequence contains'
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
    
    return $form;
}

/**
 * Implements hook_form_validate()
 *   Validate the form values for the new database to add.
 */
function tseq_database_add_form_validate($form, &$form_state)
{
    //
    if (empty($form_state['values']['Type']))
    {
        form_set_error('Type',t("Please enter the name of the Target database."));
    }
    
    //
    if (empty($form_state['values']['Version']))
    {
        form_set_error('Version',t("Please enter an appropriate version for the Target database"));
    }
    
    if (empty($form_state['values']['Location']))
    {
        form_set_error('Location',t("Please enter the path of the Target database on the filesystem"));
    }
    
}

/**
 * Implements hook_form_submit()
 *   Submits the form, adds the new database to the list 
 *   (table tseq_db_existing_locations).
 * @todo Move this function (and others like it) to tseq_databases.inc
 */
function tseq_database_add_form_submit($form, &$form_state)
{
    /*
     * Add the new database to the database
     */
    
    $newDB = array(
        'name'          => $form_state['values']['Name'],
        'type'          => $form_state['values']['Type'],
        'version'       => $form_state['values']['Version'],
        'location'      => $form_state['values']['Location'],
        'category'      => $form_state['values']['Category'],
        'web_location'  => $form_state['values']['WebLocation'],
        'count'         => $form_state['values']['Count'],
    );
    
    drupal_write_record('tseq_db_existing_locations', $newDB);
    /* Congratulate the user and let them return to the configuration screen
     * Defaults to staying at current location in case user wants to add
     * another
     */ 
    drupal_set_message("This database was successfully added");
    drupal_set_message(l('Return to Database list','admin/tripal/extension/tseq/config'));
}

/**
 * Implements hook_form
 *   Builds a form to edit an existing database.
 */
function tseq_database_edit_form($form, &$form_state,$db_id)
{
    // Build the breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>');
    $breadcrumb[] = l('Administration', 'admin');
    $breadcrumb[] = l('Tripal','admin/tripal');
    $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
    $breadcrumb[] = l('TSeq', 'admin/tripal/extension/tseq/config');
    $breadcrumb[] = l('Available Databases','admin/tripal/extension/tseq/config/db_list');
    $breadcrumb[] = l('Database ID '.$db_id, 'admin/tripal/extension/tseq/edit_db/'.$db_id);
    drupal_set_breadcrumb($breadcrumb);
    
    $query = "SELECT * FROM tseq_db_existing_locations WHERE db_id='$db_id'";
    $results = db_query($query);
    
    $rowToUpdate = $results->fetchAssoc();
    
    //Make the $db_id variable available to validate() and submit()
    //$db_id_s = &drupal_static(__FUNCTION__);
    //$db_id_s = $db_id;
     
   $form['Name'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Name',
        '#size'         =>  '50',
        '#description'  =>  'The Name of the target database.',
        '#required'     =>  true,
        '#default_value'      =>  t($rowToUpdate['name']),
        '#prefix'       => '<h2>Current values</h2>'
    );
    
    $form['Type'] = array(
        '#type'         => 'select',
        '#title'        => 'Type',
        '#description'  => 'The type of target database being added',
        '#options'      => array(
            'Protein'       => t('Protein'),
            'Gene'          => t('Gene'),
            'Genome'        => t('Genome'),
        ),
        '#default_value'      =>  t($rowToUpdate['type']),
    );
    
    $form['Version'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Version',
        '#size'         =>  '7',
        '#description'  =>  'The Version of the target database',
        '#required'     =>  true,
        '#default_value'      =>  t($rowToUpdate['version']),
    );
    
    $form['Location'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Location',
        '#size'         =>  '120',
        '#description'  =>  'The path to the indexed database on disk (accessible on the remote server)',
        '#required'     =>  true,
        '#default_value'      =>  t($rowToUpdate['location']),
    );
    
    $form['WebLocation'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Web Location (Optional)',
        '#size'         =>  '120',
        '#description'  =>  'If the original sequence (non-indexed) is publicly available for download, set the URL here (FTP, HTTP)',
        '#required'     =>  false,
        '#default_value'      =>  t($rowToUpdate['web_location']),
    );
    
    $form['Count'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Sequence Count (Optional)',
        '#size'         => '10',
        '#description'  => 'How many genes, proteins, or scaffolds the sequence contains',
        '#default_value'    => t($rowToUpdate['count']),
    );
    
    $form['db_id'] = array(
        '#type'         =>  'value',
        '#value'        =>  $db_id,
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Update values'),
    );
 
    return $form;//$rowToUpdate['db_id'].': '.$rowToUpdate['name'].': '.$rowToUpdate['version'];;
}

/**
 * Implements hook_form_validate()
 */
function tseq_database_edit_form_validate($form, &$form_state)
{
   /*
    * Basic validation (form empty?)
    * Todo: Check whether the entry already exists (key: name, version)
    * 
    */
    
    
   if (empty($form_state['values']['Type']))
    {
        form_set_error('Type',t("Please enter the name of the Target database."));
    }
    
    if (empty($form_state['values']['Version']))
    {
        form_set_error('Version',t("Please enter an appropriate version for the Target database"));
    }
    
    if (empty($form_state['values']['Location']))
    {
        form_set_error('Location',t("Please enter the path of the Target database on the filesystem"));
    } 
}

/**
 * Implements hook_form_submit()
 */
function tseq_database_edit_form_submit($form, &$form_state)
{    
    $updatedFields = array(
        'db_id'         => $form_state['values']['db_id'],
        'name'          => $form_state['values']['Name'],
        'type'          => $form_state['values']['Type'],
        'version'       => $form_state['values']['Version'],
        'location'      => $form_state['values']['Location'],
        'web_location'  => $form_state['values']['WebLocation'],
        'count'         => $form_state['values']['Count'],
    );
    
    drupal_write_record('tseq_db_existing_locations', $updatedFields, 'db_id');
    
    
    
    //Remove the $db_id variable from &drupal_static
    //drupal_static_reset();
    drupal_set_message("This database was successfully updated");
    drupal_set_message(l('Return to Database list','admin/tripal/extension/tseq/config'));
}

/**
 * Implements hook_form()
 *   Creates a form for the admin to delete an existing database from the list.
 */
function tseq_database_delete_form($form, &$form_state,$db_id)
{
    $form['Instructions'] = array(
        '#type'         => 'fieldset',
        '#title'        => 'Are you sure?',
        '#collapsible'  => FALSE,
        '#description'  => t('This will only delete the entry, it will not delete the file from the server'),
    );
    
    $form['db_id'] = array(
        '#type'         => 'value',
        '#value'        => $db_id
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
    );
    
    return $form;
}

/**
 * Implements hook_form_validate()
 * @todo This does nothing, delete?
 */
function tseq_database_delete_form_validate($form, &$form_state)
{
    
}

/**
 * Implements hook_form_submit()
 */
function tseq_database_delete_form_submit($form, &$form_state)
{
    /*
     * Delete the offending database from the database
     */
    db_query('DELETE FROM tseq_db_existing_locations where db_id = '.$form_state['values']['db_id']);
    drupal_goto('/admin/tripal/extension/tseq/config');
}

/**
 * Implements hook_form()
 *   Builds a form to export the database list.
 */
function tseq_database_export_form($form, &$form_state)
{
    $form['Type'] = array(
        '#type'         => 'select',
        '#title'        => 'Type',
        '#description'  => 'The type of target database being added',
        '#options'      => array(
            'csv'       => t('CSV (Comma Separated Values)'),
            'xls'       => t('Excel XLS (Pre-2007)'),
            'xlsx'      => t('Excel XLSX (2007-Current)'),
        ),
    );
    
    $form['submit_button'] = array(
        '#type'     => 'submit',
        '#value'    => t('Export'),
    );
    
    return $form;
}

/**
 * Implements hook_form_validate()
 * @todo This does nothing, delete?
 */
function tseq_database_export_form_validate($form, &$form_state)
{
    
}

/** 
 * Implements hook_form_submit()
 *   Initiates the export of the database list.
 */
function tseq_database_export_form_submit($form, &$form_state)
{
    $db = TSeqDatabases::getDatabases('%');
    
    if (tseq_db_export($db, $form_state['values']['Type']))
    {
       drupal_set_message("Databases successfully exported"); 
    }
    else
    {
        drupal_set_message("Database list failed to export.", 'error');
    }
    
    //$form_state['redirect'] = 'admin/tripal/extension/tseq';
}

/**
 * Implements hook_form()
 *   Builds a form to import a database list.
 */
function tseq_database_import_form($form, &$form_state)
{
    $form['FileSelect'] = array(
        '#type'         => 'managed_file',
        '#title'        => 'Import a file of databases',
        '#size'         => '50',
        '#description'  => 'The file must be of type <code>.csv</code>.',
        '#required'     => true,
        '#upload_validators' => array(
            'file_validate_extensions' => array('csv xls xlsx'),
        ),
    );
    
    $form['fileHasHeaders'] = array(
        '#type'         => 'checkbox',
        '#title'        => 'This data has headers',
        '#description' => 'Click here if the file has headers',
        '#states' => array(
            'visible' => array(
                ':input[name="FileSelect"]' => array('value' => 'upload'),
            ),
        ),
    ); 
    
    $form['userValidate'] = array(
        '#type'         => 'checkbox',
        '#title'        => 'This looks OK',
        '#description'  => 'Make sure your data is being loaded correctly.',
        '#disabled'     => FALSE,
        '#states' => array(
            'visible' => array(
                ':input[name="dda"]' => array('d' => 'd'),
            ),
        ),
        '#weight'       => '2',
    );
    
    $form['submit_button'] = array(
        '#type'     => 'submit',
        '#value'    => t('Import'),
        '#weight'   => '3',
    );    
    
    return $form;
}
/**
 * Implements hook_form_validate()
 *   Ensures that the user uploaded a correct file (CSV format, required 
 *   headers)
 * 
 * @todo Notify user of improperly formatted csv
 * @todo Detect improperly formatted csv (bad # columns, no columns
 * @todo Perhaps detect if csv is actually tab-delimited
 */
function tseq_database_import_form_validate($form, &$form_state)
{    
    $imported_file = file_load($form_state['values']['FileSelect']);
    $imported_file_URI = drupal_realpath($imported_file->uri);
    
    if ($imported_file_handle = fopen($imported_file_URI, 'r'))
    {
        $k = 0;
        while (!feof($imported_file_handle))
        {
            $file_lines[$k] = fgetcsv($imported_file_handle);
            $k++;
        }
        // Close the file 
        fclose($imported_file_handle);
        
        // Save values to form
        $form_state['values']['file_lines'] = $file_lines;

        $table = "<table>"
                . "<tr><th>Type</th>"
                . "<th>Name</th>"
                . "<th>Version</th>"
                . "<th>Location</th>"
                . "<th>Category</th>"
                . "<th>Web Location</th>"
                . "<th>Count</th>"
                . "</tr>";
        $table .= "<tr>";
        // generate table from ingested thingy
        // Check if the user specified that there are headers
        if ($form_state['values']['fileHasHeaders'])
        {
            $i = 1;
        }
        else
        {
            $i = 0;
        }
        for ($i; $i < count($file_lines);$i++)
        {
            $table .="<tr>";
            for ($j = 0; $j < count($file_lines[$i]);$j++)
            {
                $table .="<td>".trim($file_lines[$i][$j])."</td>";
            }
            $table .="</tr>";
            //}
            /*else
            {
            $table .= "<td>".$fileLines[$i]."</td>";
            }*/
        }
        $table .= "</table>";
        drupal_set_message($table);
    }
    
    if (empty($form_state['values']['userValidate']))
    {
        form_set_error('userValidate',t("Make sure the data above is correct, then click Import"));
    }
}

/**
 * 
 * Implements hook_form_submit()
 *  This submit function adds any new databases from the imported file.
 *  New databases are added in proper form to the tseq_db_existing_locations
 *  table.
 */
function tseq_database_import_form_submit($form, &$form_state)
{
    $file_lines = $form_state['values']['file_lines'];
    if ($form_state['values']['fileHasHeaders'])
    {
        $i = 1;
    }
    else
    {
        $i = 0;
    }
    for ($i; $i < count($file_lines);$i++)
    {
        $new_db = array(
            'name'          => $file_lines[$i][1],
            'type'          => $file_lines[$i][0],
            'version'       => $file_lines[$i][2],
            'location'      => $file_lines[$i][3],
            'category'      => $file_lines[$i][4],
            'web_location'  => $file_lines[$i][5],
            'count'         => $file_lines[$i][6]
        );
        drupal_write_record('tseq_db_existing_locations', $new_db);
    }
    drupal_set_message("List of databases has been successfully imported.");
}





