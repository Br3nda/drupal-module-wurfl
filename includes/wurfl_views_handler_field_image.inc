<?php 

// $Id  $

class wurfl_views_handler_field_image extends views_handler_field_markup {
	function construct() {
	   parent::construct();
	}
	
	function render($values) {
		
    if ($values->id) {
    	$file = WURFL_IMAGES . "{$values->id}.gif";
    	if(file_exists($file)) {
    		return theme('image', $file);
    	} else {
    		return theme('image', WURFL_NODEVICE_IMAGE);
    	}
    }
  }
  
	function query() {
	   //$this->add_additional_fields();
	}  
}
