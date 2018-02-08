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
    $breadcrumb[] = l('Page '.$page, 'admin/tripal/extension/diamond/'.$page);
    drupal_set_breadcrumb($breadcrumb);
    
    /*
    foreach ($databaseList as $db)
    {
        $edit = l('Edit','admin/tripal/extension/diamond/edit_db/' . $db['db_id']);
        $delete = l('Delete','admin/tripal/extension/diamond/delete_db/' . $db['db_id']);
        
        $returnedDBs[] = array($i,$db['name'],$db['version'],$db['type'],$db['location'],$edit.' | '.$delete);
        $i++;
    }
     * 
     */
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
            
            //$readable = tripal_remote_check_files($db);   //Not implemented
            $returnedDBs[] = array($i+1,$db[$i]['name'],$db[$i]['version'],$db[$i]['type'],$db[$i]['location'],$readable,$edit.' | '.$delete);
        }
    }
    //$emptyCase = (($page * 10) % count($db) > 10) ? t('No databases have been added yet.' : t('Not that many databases!');
    if (($page * 10) % count($db) < 10)
    {
        $emptyCase = l('There aren\'t that many databases yet. Click here to go back to the list','admin/tripal/extension/diamond');
    }
    else
    {
        $emptyCase = t('No databases have been added yet.');
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
        '#weight'   => '2',
    );
    
    $contents['EI_links'] = array(
        '#markup'   => t('<a href=\"midspin.net\">Export this list (CSV)</a>&nbsp;|&nbsp;<a href=\"midspin.net/Home/\">Import a list (CSV, Excel)</a>'),
        '#weight'    => '1',
    );
    
    // Generate the pagination links
    $pageLinks = 'Nagivate to a page: ';
    $pageLinks = $pageLinks.'<a href="diamond/">First</a>|';
    $pageLinks = $pageLinks.'<a href="diamond/'.$page.'">Next</a>';
    
    $contents['pagination'] = array(
        '#markup'   => $pageLinks,
        '#weight'   => '3',
    );
    
    /*$contents['exportLink'] = array(
        '#markup'   => array(l('Export this list', 'http://midspin.net/Home/'),t('|')),
        '#weight'    => '1',
    );*/
    
    
    
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
        'location' => $form_state['values']['Location']
    );
    
    drupal_write_record('diamond_db_existing_locations', $newDB);
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