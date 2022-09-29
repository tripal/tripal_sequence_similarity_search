<?php

namespace Drupal\tripal_seq\Controller;

Use Drupal;
Use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * This controller handles configuration for Tripal Sequence Similarity Search
 * 
 * The default page is a the list of Diamond/BLAST database files
 */

class Tripal_SeqConfig {
    
    function tripal_seq_config() {

        // Assemble the bits and pieces needed for the table.

        $header = array(
            array('data' => 'Name',                 'field' => 'tseq_ed.name',        'sort' => 'asc'),
            array('data' => 'Version',              'field' => 'tseq_ed.version'),
            array('data' => 'Type',                 'field' => 'tseq_ed.type'),
            array('data' => 'Category',             'field' => 'tseq_ed.category'),
            array('data' => 'Location (Remote)',    'field' => 'tseq_ed.location'),
            array('data' => 'Status',               'field' => 'tseq_ed.status'),
            array('data' => 'Count',                'field' => 'tseq_ed.count'),
            array('data' => 'Web?',                 'field' => 'tseq_ed.web_location'),
            array('data' => 'Actions'),
        );

        $db = \Drupal::database();

        $table_name = 'tseq_db_existing_locations';
        $query = $db->select($table_name, 'tseq_ed')
            ->extend('\Drupal\Core\Database\Query\TableSortExtender')
            ->fields('tseq_ed');

        $results = $query->orderByHeader($header)
                ->execute();
        

        $rows = [];
        while(($result = $results->fetchObject())) {
            // Prepare the actions
            $edit_url = Url::fromRoute('tripal_seq.config.db.edit', array('db_id' => $result->db_id));
            $edit_link = Link::fromTextAndUrl(t('Edit'), $edit_url)->toString();
            $edit_link = render($edit_link);

            $delete_url = Url::fromRoute('tripal_seq.config.db.delete', array('db_id' => $result->db_id));
            $delete_link = Link::fromTextAndUrl(t('Delete'), $delete_url)->toString();
            $delete_link = render($delete_link);
            $link_concat = ['#markup' => $edit_link . ' | ' . $delete_link];

            $rows[] = [
                $result->name,
                $result->version,
                $result->type,
                $result->category,
                $result->location,
                $result->status,
                $result->count,
                $result->web_location,
                \Drupal::service('renderer')->render($link_concat),
            ];
        }
        

        $output = [
            '#type' => 'table',
            '#header' => $header,
            '#rows' => $rows,
        ];

        /*return [
            '#markup' => 'There will be a table here!'
        ];
        */
        return $output;
    }

    function tripal_seq_config_categories() {

        // basic table
        $header = array(
            array('data' => 'Category Title',   'field' => 'tseq_cat.category_title', 'sort' => 'asc'),
            array('data' => 'Enabled',          'field' => 'tseq_cat.enabled'),
            array('data' => 'Actions'),
        );

        $db = \Drupal::database();

        $table_name = 'tseq_categories';
        $query = $db->select($table_name,'tseq_cat')
                ->extend('\Drupal\Core\Database\Query\TableSortExtender')
                ->fields('tseq_cat');

        $results = $query->orderByHeader($header)
                ->execute();
        $rows = [];
        while(($result = $results->fetchObject())) {
            $rows[] = [
                $result->category_title,
                $result->enabled,
                'some actions',
            ];
        }

        $output = [
            '#type' => 'table',
            '#header' => $header,
            '#rows' => $rows,
        ];

        return $output;
    }
}