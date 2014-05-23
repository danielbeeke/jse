<?php

/**
* Generates the JSON slimmed entity editing form.
*/
function jse_config_form($form, &$form_state, $jse_config, $op = 'edit') {
  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => isset($jse_config->label) ? $jse_config->label : '',
  );

  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($jse_config->name) ? $jse_config->name : '',
    '#machine_name' => array(
      'source' => array('label'),
      'exists' => 'jse_exists',
    ),
  );

  $form['endpoint_path'] = array(
    '#title' => t('Endpoint path'),
    '#type' => 'textfield',
    '#default_value' => isset($jse_config->endpoint_path) ? $jse_config->endpoint_path : '',
  );

  $form['plural'] = array(
    '#title' => t('Plural'),
    '#type' => 'textfield',
    '#default_value' => isset($jse_config->plural) ? $jse_config->plural : '',
  );

  $entity_info = entity_get_info();
  $entity_types = array();

  foreach ($entity_info as $entity_type => $entity_type_info) {
    if (isset($entity_type_info['fieldable']) && $entity_type_info['fieldable'] == TRUE) {

      foreach ($entity_type_info['view modes'] as $view_mode => $view_mode_info) {
        $entity_types_and_view_modes[$entity_type_info['label']][$entity_type . '|' . $view_mode] = $view_mode_info['label'];
      }
    }
  };

  if (isset($jse_config->referenced_entity_type) && $jse_config->view_mode) {
    $default_value = $jse_config->referenced_entity_type . '|' . $jse_config->view_mode;
  }

  $form['referenced_entity_type_and_view_mode'] = array(
    '#title' => t('Entity type to load and view mode to display'),
    '#type' => 'select',
    '#options' => $entity_types_and_view_modes,
    '#default_value' => isset($default_value) ? $default_value : '',
  );

  $fields = field_info_fields();

  $compatible_reference_field_types = array(
    'entityreference'
  );

  $enabled_references = array();

  if (isset($jse_config->enabled_references)) {
    $enabled_references = json_decode($jse_config->enabled_references, TRUE);
  }

  foreach ($fields as $field_name => $field_info) {
    if (in_array($field_info['type'], $compatible_reference_field_types)) {
      $form['relation_' . $field_name] = array(
        '#type' => 'checkbox',
        '#title' => $field_name,
        '#name' => 'relation_' . $field_name,
        '#default_value' => isset($enabled_references[$field_name]) ? TRUE: FALSE,
      );

      $form['relation_alias_' . $field_name] = array(
        '#type' => 'textfield',
        '#title' => t('alias'),
        '#default_value' => isset($enabled_references[$field_name]) ? $enabled_references[$field_name]: FALSE,
        '#states' => array(
          'visible' => array(
            ':input[name="' . 'relation_' . $field_name . '"]' => array('checked' => TRUE),
          ),
        ),
      );
    }
  }

  $form['actions'] = array('#type' => 'actions');

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 40,
  );

  return $form;
}

/**
* Form API submit callback for the JSON slimmed entity form.
*/
function jse_config_form_submit(&$form, &$form_state) {
  $combined_value_exploded = explode('|', $form_state['values']['referenced_entity_type_and_view_mode']);
  $form_state['values']['referenced_entity_type'] = $combined_value_exploded[0];
  $form_state['values']['view_mode'] = $combined_value_exploded[1];
  $references = array();

  foreach($form_state['values'] as $form_item_key => $value) {
    if (substr($form_item_key, 0, 8) == 'relation' && $value && substr($form_item_key, 0, 14) != 'relation_alias') {
      $references[substr($form_item_key, 9)] = $form_state['values']['relation_alias_' . substr($form_item_key, 9)];
    }
  }

  $form_state['values']['enabled_references'] = json_encode($references);

  $jse_config = entity_ui_form_submit_build_entity($form, $form_state);

  $jse_config->save();

  menu_rebuild();

  $form_state['redirect'] = 'admin/structure/jse';
}
