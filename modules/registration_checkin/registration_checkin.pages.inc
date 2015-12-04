<?php
/**
 * @file
 * Page callbacks for Entity Registration Checkin.
 */

/**
 * Page callback: Show a list of active registrations that can be checked in.
 */
function registration_checkin_list_page($entity_type, $entity) {
  // Create table structure, header.
  $header = array(
    array(
      'data' => t('Registrant'),
    ),
    array(
      'data' => t('Email'),
    ),
    array(
      'data' => t('State'),
    ),
    array(
      'data' => t('Actions'),
    ),
  );

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = entity_label($entity_type, $entity);

  // Fetch registrations.
  $query = new EntityFieldQuery();
  $result = $query
    ->entityCondition('entity_type', 'registration')
    ->propertyCondition('entity_id', $entity_id)
    ->propertyCondition('entity_type', $entity_type)
    ->propertyOrderBy('created', 'DESC')
    ->pager(20)
    ->execute();

  // Build rows if we have registrations for the event.
  if (!empty($result['registration'])) {
    $registrations = registration_load_multiple(array_keys($result['registration']));

    // Build registrant rows.
    $rows = array();
    foreach ($registrations as $registration) {
      // Result basics.
      $wrapper = entity_metadata_wrapper('registration', $registration);
      $registrant_type = $wrapper->registrant->type();
      $registrant = $wrapper->registrant->value();

      // Link the registrant name.
      $registrant_entry = theme('registration_registrant_link', array(
        'registrant_type' => $registrant_type,
        'registrant' => $registrant,
      ));

      // State drop-down.
      $state_selection = drupal_get_form('registration_checkin_state_form', $registration);

      // Check-in button.
      $checkin_button = drupal_get_form(
        'registration_checkin_checkin_action_form', $registration
      );

      // Build the row.
      $rows[] = array(
        $registrant_entry,
        l($wrapper->registrant_mail->value(), 'mailto:' . $wrapper->registrant_mail->value()),
        render($state_selection),
        render($checkin_button),
      );
    }

    // Generate markup for the registrant table.
    $table = array(
      'header' => $header,
      'rows' => $rows,
    );
    $registrant_markup = theme('table', $table) . theme('pager');
  }
  else {
    $registrant_markup = t('There are no registrants for %name', array('%name' => $label));
  }

  // Get the search form.
  $search_form = drupal_get_form('registration_checkin_search_form', $entity_type, $entity_id);

  // DEBUG
  // $form_state = array(
  //   'values' => array(
  //     'search_keys' => 'al ri'
  //   ),
  // );
  // _do_thing($search_form, $form_state);

  // Wrap the table in with an ID'd div so we can update the results with AJAX.
  $registrant_table_build = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('registrant-table'),
    ),
    'markup' => array(
      '#markup' => $registrant_markup,
    )
  ); 

  // Create a button to checkin an unregistered attendee
  // by registering them as immediately attending.
  $new_checkin_button = array(
    '#theme' => 'link',
    '#text' => t('New Registrant Check-in'),
    '#path' => $entity_type . '/' . $entity_id . '/register/attended',
    '#options' => array(
      'attributes' => array('class' => 'button'),
      'html' => FALSE,
    ),

  );

  // Build the page as a render array.
  $page = array(
    'content' => array(
      'new_button' => $new_checkin_button,
      'search_form' => $search_form,
      'registrant_table' => $registrant_table_build,
    )
  );
  return $page;
}
