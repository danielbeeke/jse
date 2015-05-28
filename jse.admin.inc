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

  $callbacks = module_invoke_all('jse_info');
  $callback_options = array('_none' => t('-Select-'));

  foreach ($callbacks as $callback_key => $callback_info) {
    $callback_options[$callback_key] = $callback_info['label'];
  }

  $form['callback'] = array(
    '#type' => 'select',
    '#options' => $callback_options,
    '#default_value' => isset($jse_config->callback) ? $jse_config->callback : '_none',
  );

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
  $jse_config = entity_ui_form_submit_build_entity($form, $form_state);

  $jse_config->save();

  menu_rebuild();

  $form_state['redirect'] = 'admin/structure/jse';
}
