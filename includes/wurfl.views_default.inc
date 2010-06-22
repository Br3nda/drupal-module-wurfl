<?php

/**
 * @file
 * View for listing devices by wurfl.
 */

function wurfl_views_default_views() {
	$view = new view;
	$view->name = 'wurfl';
	$view->description = '';
	$view->tag = '';
	$view->view_php = '';
	$view->base_table = 'wurfl_devices';
	$view->is_cacheable = FALSE;
	$view->api_version = 2;
	$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
	$handler = $view->new_display('default', 'Defaults', 'default');
	$handler->override_option('fields', array(
	  'id' => array(
	    'label' => 'Device ID',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'id',
	    'table' => 'wurfl_devices',
	    'field' => 'id',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'user_agent' => array(
	    'label' => 'User Agent',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '128',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'user_agent',
	    'table' => 'wurfl_devices',
	    'field' => 'user_agent',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'brand_name' => array(
	    'label' => 'Brand Name',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'brand_name',
	    'table' => 'wurfl_devices',
	    'field' => 'brand_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	  'model_name' => array(
	    'label' => 'Model Name',
	    'alter' => array(
	      'alter_text' => 0,
	      'text' => '',
	      'make_link' => 0,
	      'path' => '',
	      'link_class' => '',
	      'alt' => '',
	      'prefix' => '',
	      'suffix' => '',
	      'target' => '',
	      'help' => '',
	      'trim' => 0,
	      'max_length' => '',
	      'word_boundary' => 1,
	      'ellipsis' => 1,
	      'html' => 0,
	      'strip_tags' => 0,
	    ),
	    'empty' => '',
	    'hide_empty' => 0,
	    'empty_zero' => 0,
	    'exclude' => 0,
	    'id' => 'model_name',
	    'table' => 'wurfl_devices',
	    'field' => 'model_name',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	));
	$handler->override_option('sorts', array(
	  'id' => array(
	    'order' => 'ASC',
	    'id' => 'id',
	    'table' => 'wurfl_devices',
	    'field' => 'id',
	    'override' => array(
	      'button' => 'Override',
	    ),
	    'relationship' => 'none',
	  ),
	));
	$handler->override_option('access', array(
	  'type' => 'none',
	));
	$handler->override_option('cache', array(
	  'type' => 'none',
	));
	$handler->override_option('items_per_page', 20);
	$handler = $view->new_display('page', 'Devices', 'page_1');
	$handler->override_option('title', 'WURFL Devices');
	$handler->override_option('use_ajax', TRUE);
	$handler->override_option('use_pager', '1');
	$handler->override_option('style_plugin', 'table');
	$handler->override_option('path', 'admin/build/wurfl/list');
	$handler->override_option('menu', array(
	  'type' => 'none',
	  'title' => '',
	  'description' => '',
	  'weight' => 0,
	  'name' => 'navigation',
	));
	$handler->override_option('tab_options', array(
	  'type' => 'none',
	  'title' => '',
	  'description' => '',
	  'weight' => 0,
	  'name' => 'navigation',
	));
	

  $views[$view->name] = $view;
  return $views;
}