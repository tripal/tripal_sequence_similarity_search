<?php

function tripal_diamond_config_page_build($page)
{
    /*
     * Search/Filter the table of databases below (todo)
     */
    //pumpkin
    
    /*
     * The table of currently stored databases
     */
    $headers = array('','Name','Version','Type','Location (remote)','Accessible', 'Actions');
    $rows = array();
    /*
     * Getting details from the database
     * @todo
     *  Handle database errors
     */
    $db = DiamondDatabases::getDatabases('%');
    $returnedDBs = array();
    
    // Deal with bad numbers 
    if ($page == 'tripal_diamond_config_page' || $page <= 0)
    {
        $page = 1;
    }
    
        //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Diamond', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/diamond');
    $breadcrumb[] = l('Database list, page '.$page, 'admin/tripal/extension/diamond/'.$page);
    drupal_set_breadcrumb($breadcrumb);
    
    /*
     * Check to see if the listed resource (Diamond file) is accessible
     * @todo
     *  this
     */
    /*foreach ($databaseList as $db)
    {
        $edit = l('Edit','admin/tripal/extension/diamond/edit_db/' . $db['db_id']);
        
        $accessible[] = array($i,$db['name'],$db['version'],$db['type'],$db['location'],$edit.' | '.$delete);
        $i++;
    }*/
    
    // Generate link to add another database
    $contents['addLink'] = array(
        '#markup'   => l('Add an existing database','./admin/tripal/extension/diamond/add_db'),
        '#weight'   => '1',
    );
    
    $j = $page * 10;
    $i = $j - 10;
    for($i; $i < $j; $i++)
    {
        if ($i < count($db))
        {
            $edit   = l('Edit','admin/tripal/extension/diamond/edit_db/' .
                    $db[$i]['db_id']);       
            $delete = l('Delete','admin/tripal/extension/diamond/delete_db/' .
                    $db[$i]['db_id']);
            
            $readable = (string)is_readable($db[$i]['location']);
            $readable = $readable ? "Yes":"No";
            //$readable = tripal_remote_check_files($db);   //Not implemented
            $returnedDBs[] = array($i+1,$db[$i]['name'],$db[$i]['version'],$db[$i]['type'],$db[$i]['location'],$readable,$edit.' | '.$delete);
        }
    }
    
    //Check to see if there are databases
    $emptyCase = '';
    if (count($db) == 0)
    {
        $emptyCase = t('No databases have been added yet.');
    }
    else if (($page * 10) % count($db) < 10)
    {
        $emptyCase = l('There aren\'t that many databases yet. Click here to go back to the list','admin/tripal/extension/diamond');
    }
    
    $table_vars = array(
        'header'      => $headers, 
        'rows'        => $returnedDBs,
        'attributes'  => array(),
        'sticky'      => TRUE,
        'colgroups'   => array(),
        'empty'       => $emptyCase,
    );
    
    $contents['resource_list'] = array(
        '#markup'   => theme('table', $table_vars),
        '#weight'   => '4',
    );
    
    /*
     * Import/Export links. currently disabled
     * @todo
     *  implement this properly using Drupal (or Tripal?) content types
     */
    /*$contents['EI_links'] = array(
        '#markup'   => t('<a href="'.base_path().'admin/tripal/extension/diamond/export">Export this list (CSV)</a>&nbsp;|&nbsp;<a href="'.base_path().'admin/tripal/extension/diamond/import">Import a list (CSV, Excel)</a>'),
        '#weight'    => '3',
    );*/
    
    // Generate the pagination links
    $pageLinks = '';
    if (count($db) > 10)
    {
        $pageLinks = 'Nagivate to a page: ';
        $pageLinks = $pageLinks.'<a href="diamond/1">1</a>';
        $numPages = ceil(count($db) / 10);
        for ($i = 2; $i <= $numPages; $i++)
        {
            $pageLinks = $pageLinks.' | <a href="diamond/'.$i.'">'.$i.'</a>';
        }
    }
    
    $contents['pagination'] = array(
        '#markup'   => $pageLinks,
        '#weight'   => '10',
    );
    
    return $contents;
}

/*
 * Add a database to the list (form)
 */
function tripal_diamond_database_add_form($form, &$form_state)
{
    //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Diamond', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/diamond');
    $breadcrumb[] = l('Add database', 'admin/tripal/extension/diamond/add');
    drupal_set_breadcrumb($breadcrumb);
    
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
    );
    
    $form['Version'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Version',
        '#size'         => '7',
        '#description'  => 'The version of the target database',
        '#required'     => true,
    );
    
    $form['Location'] = array(
        '#type'         => 'textfield',
        '#title'        => 'File location',
        '#size'         => '120',
        '#description'  => 'The path to the database on disk',
        '#required'     => true,
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
    
    return $form;
}

function tripal_diamond_database_add_form_validate($form, &$form_state)
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

function tripal_diamond_database_add_form_submit($form, &$form_state)
{
    /*
     * Add the new database to the database
     */
    
    $newDB = array(
        'name' => $form_state['values']['Name'],
        'type' => $form_state['values']['Type'],
        'version' => $form_state['values']['Version'],
        'location' => $form_state['values']['Location'],
    );
    
    drupal_write_record('diamond_db_existing_locations', $newDB);
    /* Congratulate the user and let them return to the configuration screen
     * Defaults to staying at current location in case user wants to add
     * another
     */ 
    drupal_set_message("This database was successfully added");
    drupal_set_message(l('Return to Database list','admin/tripal/extension/diamond'));
}
/*
 * Edit a database in the list (form)
 */
function tripal_diamond_database_edit_form($form, &$form_state,$db_id)
{
    //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('Diamond', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/diamond');
    $breadcrumb[] = l('Edit database', 'admin/tripal/extension/diamond/edit_db/'.$db_id);
    drupal_set_breadcrumb($breadcrumb);
    
    $query = "SELECT * FROM diamond_db_existing_locations WHERE db_id='$db_id'";
    $results = db_query($query);
    
    $rowToUpdate = $results->fetchAssoc();
    
    //Make the $db_id variable available to validate() and submit()
    //$db_id_s = &drupal_static(__FUNCTION__);
    //$db_id_s = $db_id;
     
   $form['Name'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Name',
        '#size'         =>  '50',
        '#description'  =>  'The Name of the target database (.dmnd) file.',
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
            'Nucleotide'    => t('Nucleotide'),
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
        '#description'  =>  'The path to the database on disk',
        '#required'     =>  true,
        '#default_value'      =>  t($rowToUpdate['location']),
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

function tripal_diamond_database_edit_form_validate($form, &$form_state)
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

function tripal_diamond_database_edit_form_submit($form, &$form_state)
{
    /*db_update('diamond_db_existing_locations') 
        ->fields(array(
            'name' => $form_state['values']['Name'],
            'type' => $form_state['values']['Type'],
            'version' => $form_state['values']['Version'],
            'location' => $form_state['values']['Location'],
        ))
        ->condition('db_id', $form_state['values']['db_id'])
        ->execute();
    echo("Hello");
     * 
     */
    
    $updatedFields = array(
        'db_id'     => $form_state['values']['db_id'],
        'name'      => $form_state['values']['Name'],
        'type'      => $form_state['values']['Type'],
        'version'   => $form_state['values']['Version'],
        'location'  => $form_state['values']['Location'],
    );
    
    drupal_write_record('diamond_db_existing_locations', $updatedFields, 'db_id');
    
    
    
    //Remove the $db_id variable from &drupal_static
    //drupal_static_reset();
    drupal_set_message("This database was successfully updated");
    drupal_set_message(l('Return to Database list','admin/tripal/extension/diamond'));
}
/*
 * Delete a database in the list (form)
 */
function tripal_diamond_database_delete_form($form, &$form_state,$db_id)
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

function tripal_diamond_database_delete_form_validate($form, &$form_state)
{
    
}

function tripal_diamond_database_delete_form_submit($form, &$form_state)
{
    /*
     * Delete the offending database from the database
     */
    db_query('DELETE FROM diamond_db_existing_locations where db_id = '.$form_state['values']['db_id']);
    drupal_goto('/admin/tripal/extension/diamond');
}

function tripal_diamond_database_export_form($form, &$form_state)
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
    
    /*$form['backLink'] = array(
        '#type'     => 'markup',
        '#markup'    => t('<br /><br /><a href="'.base_path().'admin/tripal/extension/diamond/">Go back</a>')
    );*/
    
    return $form;
}

function tripal_diamond_database_export_form_validate($form, &$form_state)
{
    
}

function tripal_diamond_database_export_form_submit($form, &$form_state)
{
    $db = DiamondDatabases::getDatabases('%');
    
    if (tripal_diamond_db_export($db, $form_state['values']['Type']))
    {
       drupal_set_message("Databases successfully exported"); 
    }
    else
    {
        drupal_set_message("Database list failed to export.", 'error');
    }
    
    //$form_state['redirect'] = 'admin/tripal/extension/diamond';
}

function tripal_diamond_database_import_form($form, &$form_state)
{
    $form['FileSelect'] = array(
        '#type'         => 'managed_file',
        '#title'        => 'Import a file of databases',
        '#size'         => '50',
        '#description'  => 'The file must be of type <code>.csv</code>, <code>.xls</code>, or <code>.xlsx</code>. Show row information',
        '#required'     => true,
        '#upload_validators' => array(
            'file_validate_extensions' => array('csv xls xlsx'),
        ),
    );
    
    /*
     * May not be necessary - user may just have to point to .fa* files and 
     * let the app server create the index every time (if write access for
     * creating the diamond indexes is an issue. 
     */
    $form['indexCheck'] = array(
        '#type'         => 'checkbox',
        '#title'        => 'Automatically index',
        '#description' => 'Click here to automatically create a Diamond index',
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

function tripal_diamond_database_import_form_validate($form, &$form_state)
{    
    $fid = $form_state['values']['FileSelect'];
    
    $importFile = file_load($fid);
    //str_replace("temporary://","",);
    $fileName = "/tmp/".str_replace("temporary://","",$importFile->uri);
    
    $theFile = fopen($fileName,"r");
    $k = 0;
    while (!feof($theFile))
    {
        $fileLines[$k] = fgetcsv($theFile);
        $k++;
    }
    
    $table = "<table><tr><th>Type</th><th>Name</th><th>Version</th><th>Location</th></tr>";
    $table .= "<tr>";
    for ($i = 0; $i < count($fileLines);$i++)
    {
        $table .="<tr>";
        for ($j = 0; $j < count($fileLines[$i]);$j++)
        {
            $table .="<td>".$fileLines[$i][$j]."</td>";
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
   
    
    if (empty($form_state['values']['userValidate']))
    {
        form_set_error('userValidate',t("Make sure the data above is correct, then click Import"));
    }
}

function tripal_diamond_database_import_form_submit($form, &$form_state)
{
    drupal_set_message("Pumpkin importation");
}