<?php
// $Id:

/**
 * @file
 *   Configuration file for the WURFL module
 */
 
/**
 * Settings page 
 *
 */
function wurfl_settings() {
  $output = '';
  // Check if the library is installed.
  $library = wurfl_check_library();
  if($library === FALSE) {
    $output .= theme('wurfl_status', t('The WURFL PHP library cannot be found. Read the INSTALL.txt file for instructions.'), 'error');   
    return $output;
  }
  else {
    $output .= theme("wurfl_status", t('The WURFL library is successfully installed in ' . $library));
  }
  
  // Check if a resources directory is created
  if(!file_exists("$library/resources") || !file_exists("$library/cache")) {
    $output .= theme("wurfl_status", t("Please create the 'resources' and 'cache' folder in !library. Copy wurfl.xml and wurfl-config.xml in this folder.", array('!library' => $library)), 'error');
    return $output;
  }
  else {
    // check if the wurfl xml file exist
    $xml =  "$library/resources/wurfl.xml";
    if(!file_exists($xml)) {
      $output .= theme("wurfl_status", t('The WURFL device file (wurfl.xml) is not found in !library/resources. Read the INSTALL.txt file for instructions.',  array('!library' => $library)), 'error');   
      return $output;
    }
    else {
      $output .= theme("wurfl_status", t('The WURFL device file (wurfl.xml) is found in !library/resources/wurfl.xml',  array('!library' => $library)));
      // Check if the wurfl-config file exist
      $config = "$library/resources/wurfl-config.xml";
      if(!file_exists($config)) {
        $output .= theme("wurfl_status", t('The WURFL configuration file (wurfl-config.xml) cannot be found in $library/resources. Read the INSTALL.txt file for instructions.'), 'error');  
        return $output; 
      }
      else {
        $output .= theme("wurfl_status", t('The WURFL library is successfully installed in $library/resources/wurfl-config.xml'));
        wurfl_boot();
        global $wurflManager;
        $info = $wurflManager->getWURFLInfo();
        $output .= '<h2>WURFL Version information</h2>';
        $output .= '<dl>';
        $output .= "<dt>Version: </dt> <dd>" . $info->__get('version') . "</dd>";  
        $output .= "<dt>Last Updated: </dt> <dd> " . $info->__get('lastUpdated') . "</dd>";   
        $output .= "<dt>Official URL: </dt> <dd> " . $info->__get('officialURL') . "</dd>";  
      } 
    }
  }
  return $output;
}

/**
 * Implementation of hook_theme
 */
function wurfl_theme($existing, $type, $theme, $path){
  $items['wurfl_status'] = array(
    'arguments' => array('message' => NULL, 'type' => "status"),
  );
  return $items;
}

/**
 * Theme the status messages
 */
function theme_wurfl_status($message, $type = "status") {
  $output = "<div class=\"message $type\">\n";
  $output .= "<ul>\n";
  $output .= "<li>$message</li>\n";
  $output .= "</ul>\n";
  $output .= "</div>";

  return $output;
}

function wurfl_configuration_form($form_state) {
  $form['lib_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to WURFL data'),
    '#description' => t("Absolute path on this server to your installation of the WURFL database.  Module will look for directories called <b>resources</b> and <b>WURFL</b> using this path."),
    '#default_value' => variable_get('wurfl_data_path', dirname(__FILE__) . '/lib/wurfl'),
  );
  $form['advanced'] = array(
    '#type' => 'markup',
    '#value' => '<p>' . t('Advanced configurations can be done in the wurfl-config.xml file, located in the resources folder of your WURFL project.') . '</p>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function wurfl_configuration_form_submit($form, &$form_state) {
  variable_set('wurfl_data_path', $form_state['values']['lib_path']);
  drupal_set_message(t('The settings have been saved.'));
}
