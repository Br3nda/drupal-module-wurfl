<?php 

class wurfl_views_handler_field_image extends views_handler_field_markup {
	function construct() {
	   parent::construct();
	}
	
	function render($values) {
    if ($values->id) {
      return theme('image', WURFL_IMAGES . "{$values->id}.gif");
    } else {
    	return t('No Image!');
    }
  }
  
	function query() {
	   //$this->add_additional_fields();
	}  
}
