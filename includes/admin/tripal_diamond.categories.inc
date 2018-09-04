<?php
/*
 *  Build the page to display current categories
 */
function tseq_config_categories_page_build($page)
{   
    /*
     * The table of currently stored databases
     */
    $headers = array('','Category Title','Enabled','Actions');
    $rows = array();
    /*
     * Get categories from the database
     */
    $query = 'select * from tseq_categories';
    $results = db_query($query);
    
    $categories = array();
        
    //Iterate through the returned database 
    for($i = 0; $i< $results->rowCount();$i++)
    {
      $categories[$i] = $results->fetchAssoc();
    }
        
    // Deal with bad numbers 
    if ($page == 'tseq_config_page' || $page <= 0)
    {
        $page = 1;
    }
    
        //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('TSeq', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/tseq');
    $breadcrumb[] = l('Category list, page '.$page, 'admin/tripal/extension/tseq/config/categories/'.$page);
    drupal_set_breadcrumb($breadcrumb);

    // Generate link to add another database
    $contents['addLink'] = array(
        '#markup'   => l('Add an new category','./admin/tripal/extension/tseq/add_category'),
        '#weight'   => '1',
    );
    
    $j = $page * 10;
    $i = $j - 10;
    $returnedCategories = array();
    for($i; $i < $j; $i++)
    {
        if ($i < count($categories))
        {
            $edit   = l('Edit','admin/tripal/extension/tseq/edit_category/' .
                    $categories[$i]['category_id']);       
            $delete = l('Delete','admin/tripal/extension/tseq/delete_category/' .
                    $categories[$i]['category_id']);
            
            $humanReadableBool = ($categories[$i]['enabled'] == '1') ? 'Yes' : 'No'; // Convert 1/0 into Yes/No
            $returnedCategories[] = array($i+1,$categories[$i]['category_title'],$humanReadableBool, $edit.' | '.$delete);
        }
    }
    
    //Check to see if there are databases
    $emptyCase = '';
    if (count($categories) == 0)
    {
        $emptyCase = t('No categories have been found. At least one category should exist for any databases to appear on the Submit page');
    }
    else if (($page * 10) % count($categories) < 10)
    {
        $emptyCase = l('There aren\'t that many categories yet. Click here to go back to the list','admin/tripal/extension/tseq');
    }
    
    $table_vars = array(
        'header'      => $headers, 
        'rows'        => $returnedCategories,
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
        '#markup'   => t('<a href="'.base_path().'admin/tripal/extension/tseq/export">Export this list (CSV)</a>&nbsp;|&nbsp;<a href="'.base_path().'admin/tripal/extension/tseq/import">Import a list (CSV, Excel)</a>'),
        '#weight'    => '3',
    );*/
    
    // Generate the pagination links
    $pageLinks = '';
    if (count($categories) > 10)
    {
        $pageLinks = 'Nagivate to a page: ';
        $modulePath = base_path().'admin/tripal/extension/tseq/config/categories/';
        $pageLinks = $pageLinks.'<a href="'.$modulePath.'1">1</a>';
        $numPages = ceil(count($categories) / 10);
        for ($i = 2; $i <= $numPages; $i++)
        {
            $pageLinks = $pageLinks.' | <a href="'.$modulePath.$i.'">'.$i.'</a>';
        }
    }
    
    $contents['pagination'] = array(
        '#markup'   => $pageLinks,
        '#weight'   => '10',
    );
    
    return $contents;
}


/*
 *  Add a new category
 */
function tseq_category_add_form($form, &$form_state)
{
    //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('TSeq', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/tseq');
    $breadcrumb[] = l('Add category', 'admin/tripal/extension/tseq/add_category');
    drupal_set_breadcrumb($breadcrumb);
    
    $form['category_title'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Name',
        '#size'         => '50',
        '#description'  => 'The name of the category',
        '#required'     => true,
    );
    
    $form['enabled'] = array(
        '#type'         => 'select',
        '#title'        => 'Enabled',
        '#description'  => 'Show this category on the Submit page',
        '#options'      => array(
            '1'       => t('Yes'),
            '0'        => t('No'),
        ),
        '#required'     => true,
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Add'),
    );
    
    return $form;
}

function tseq_category_add_form_validate($form, &$form_state)
{
    //
    if (empty($form_state['values']['category_title']))
    {
        form_set_error('category_title',t("Please enter the name of the category."));
    }
    /*
     * Check if the entered category title is a duplicate
     */
    $query = 'select * from tseq_categories';
    $results = db_query($query);
    
    $categories = array();
        
    //Iterate through the returned database 
    for($i = 0; $i< $results->rowCount();$i++)
    {
      $categories[$i] = $results->fetchAssoc();
    }
    for($j = 0; $j < count($categories); $j++)
    {
        if ($categories[$j]['category_title'] == $form_state['values']['category_title'])
        {
            form_set_error('category_title', t("That category already exists."));
        }
    }
    
}

function tseq_category_add_form_submit($form, &$form_state)
{
    /*
     * Add the new database to the database
     */
    
    $newCategory = array(
        'category_title' => $form_state['values']['category_title'],
        'enabled' => $form_state['values']['enabled'],
    );
    
    drupal_write_record('tseq_categories', $newCategory);
    /* Congratulate the user and let them return to the configuration screen
     * Defaults to staying at current location in case user wants to add
     * another
     */ 
    drupal_set_message("This database was successfully added");
    drupal_set_message(l('Return to Category list','admin/tripal/extension/tseq/config/categories'));
}
/*
 *  Edit an existing category
 */
function tseq_category_edit_form($form, &$form_state,$category_id)
{
    //Breadcrumb
    $breadcrumb = array();
    $breadcrumb[] = l('TSeq', '<front>');
    $breadcrumb[] = l('Config', 'admin/tripal/extension/tseq');
    $breadcrumb[] = l('Edit database', 'admin/tripal/extension/tseq/edit_category/'.$category_id);
    drupal_set_breadcrumb($breadcrumb);
    
    $query = "SELECT * FROM tseq_categories WHERE category_id='$category_id'";
    $results = db_query($query);
    
    $rowToUpdate = $results->fetchAssoc();
    
    //Make the $db_id variable available to validate() and submit()
    //$db_id_s = &drupal_static(__FUNCTION__);
    //$db_id_s = $db_id;
     
   $form['category_title'] = array(
        '#type'         =>  'textfield',
        '#title'        =>  'Category Title',
        '#size'         =>  '50',
        '#description'  =>  'The name of the category.',
        '#required'     =>  true,
        '#default_value'      =>  t($rowToUpdate['category_title']),
        '#prefix'       => '<h2>Current values</h2>'
    );
    
    $form['enabled'] = array(
        '#type'         => 'select',
        '#title'        => 'Enabled',
        '#description'  => 'Show this category on the Submit page',
        '#options'      => array(
            '1'             => t('Yes'),
            '0'             => t('No'),
        ),
        '#default_value'      =>  t($rowToUpdate['enabled']),
    );
    
    $form['category_id'] = array(
        '#type'         =>  'value',
        '#value'        =>  $category_id,
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Update values'),
    );
 
    return $form;//$rowToUpdate['db_id'].': '.$rowToUpdate['name'].': '.$rowToUpdate['version'];;
}

function tseq_category_edit_form_validate($form, &$form_state)
{
   if (empty($form_state['values']['category_title']))
    {
        form_set_error('category_title',t("Please enter the name of the category."));
    }    
}

function tseq_category_edit_form_submit($form, &$form_state)
{    
    $updatedFields = array(
        'category_id'    => $form_state['values']['category_id'],
        'category_title'    => $form_state['values']['category_title'],
        'enabled'           => $form_state['values']['enabled'],
    );
    
    drupal_write_record('tseq_categories', $updatedFields, 'category_id');
    
    
    
    //Remove the $db_id variable from &drupal_static
    //drupal_static_reset();
    drupal_set_message("This category was successfully updated");
    drupal_set_message(l('Return to Category list','admin/tripal/extension/tseq/config/categories'));
}
/*
 *  Delete an existing category
 */
function tseq_category_delete_form($form, &$form_state,$category_id)
{
    $form['Instructions'] = array(
        '#type'         => 'fieldset',
        '#title'        => 'Are you sure?',
        '#collapsible'  => FALSE,
        '#description'  => t('This will only delete the category. Any databases under this category will no longer be listed (but will still exist).'),
    );
    
    $form['category_id'] = array(
        '#type'         => 'value',
        '#value'        => $category_id
    );
    
    $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
    );
    
    return $form;
}

function tseq_category_delete_form_validate($form, &$form_state)
{
    
}

function tseq_category_delete_form_submit($form, &$form_state)
{
    /*
     * Delete the offending database from the database
     */
    db_query('DELETE FROM tseq_categories where category_id = '.$form_state['values']['category_id']);
    
    drupal_set_message("This category was successfully deleted");
    drupal_goto('admin/tripal/extension/tseq/config/categories');
}