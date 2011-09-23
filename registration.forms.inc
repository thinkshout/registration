<?php

/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function registration_form($form, &$form_state, $registration) {
  $form_state['registration'] = $registration;

	global $user;
	$form['mail'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Email'),
	  '#description' => t('The email to associate with this registration.'),
		'#default_value' => $user->uid ? $user->mail : '',
	  '#size' => 40,
	  '#maxlength' => 255,
		'#required' => TRUE
	);

  field_attach_form('registration', $registration, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Registration'),
  );
  
  // add a delete button for existing registration
  if(isset($registration->registration_id)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  
  if (!empty($registration->registration_id) && 
			user_access('administer registration')) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('registration_form_delete_submit'),
    );
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'node/' . $registration->nid,
  );


  return $form;
}

/**
 * Validation callback for registration_form().
 */
function registration_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['mail'])) {
		form_set_error('mail', t('The email address is invalid.'));
  }
	
  $registration = $form_state['registration'];
  // Notify field widgets to validate their data.
  field_attach_form_validate('registration', $registration, $form, $form_state);
}

/**
 * Submit callback for registration_form().
 */
function registration_form_submit($form, &$form_state) {
  $registration = $form_state['registration'];

  // Set the contact's author uid
  global $user;
  $registration->author_uid = $user->uid;
  $registration->mail = $form_state['values']['mail'];

  // Notify field widgets.
  field_attach_submit('registration', $registration, $form, $form_state);
  
  // Save the registration and redirect to the tour node.
  if (registration_save($registration)) {
    drupal_set_message(t('Thanks you, your registration has been recieved!'));
    $form_state['redirect'] = 'node/' . $registration->nid;    
  }
  else {
    drupal_set_message(t('Sorry, there has been a problem submitting your registration.'));
  }

}

/**
 * Button submit function: handle the 'Delete' button on the node form.
 */
function registration_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $registration = $form_state['registration'];
  $form_state['redirect'] = array('registration/' . $registration->registration_id . '/delete', array('query' => $destination));
}


/**
 * Menu callback -- ask for confirmation of node deletion
 */
function registration_delete_confirm($form, &$form_state, $registration) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['registration_id'] = array('#type' => 'value', '#value' => $registration->registration_id);
  return confirm_form($form,
    t('Are you sure you want to delete registration %title?', array('%title' => $registration->registration_id)),
    'registration/' . $registration->registration_id,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function registration_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $id = $form_state['values']['registration_id'];
    registration_delete_multiple(array($id));
    watchdog('registration', 'Registration %id deleted.', array('%id' => $id));
    drupal_set_message(t('Registration %id has been deleted.', array('%id' => $id)));
  }

  $form_state['redirect'] = '<front>';
}

/**
 * Return a form for sending a broadcast email to participants.
 */
function registration_registrations_broadcast_form($form, &$form_state, $node) {
  $form = array();
  $form_state['node'] = $node;

  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('The subject of the message.'),
    '#required' => TRUE,
    '#size' => 40,
    '#maxlength' => 255,
  );  
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#description' => t('Enter the message you want to send to this tour\'s participants.'),
    '#required' => TRUE,
    '#cols' => 60,
    '#rows' => 5,
  );
  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  
  return $form;
}

/**
 * Submit handler for mkc_tour_registrations_broadcast_form.
 */
function registration_registrations_broadcast_form_submit($form, &$form_state) {
  global $language;
  $node = $form_state['node'];
  
  // grab all registrations for this tour
  $query = new EntityFieldQuery();
  $entities = $query->entityCondition('entity_type', 'registration')
                    ->entityCondition('bundle', 'registration')
                    ->propertyCondition('nid', $node->nid)
                    ->execute();

  if(!empty($entities)) {
    $recipients = array();
    
    // load registrations and build an array of recipients
    $registrations = mkc_registration_load_multiple(array_keys($entities['registration']));    
    foreach($registrations as $registration) {
      if (isset($registration->email[LANGUAGE_NONE][0]['value'])) {
        $recipients[] = $registration->email[LANGUAGE_NONE][0]['value'];        
      }
    }
    
    // send the email and communicate results
    $params = array(
      'subject' => $form_state['values']['subject'],
      'message' => $form_state['values']['message'],      
    );
    $result = drupal_mail('registration', 'broadcast', implode(', ', $recipients), $language, $params);
    if ($result['result']) {
      drupal_set_message(t('The message has been sent to @count participants.', 
        array('@count' => count($recipients))));
    }
    else {
      drupal_set_message(
        t('There was an error sending the message.'), 
        $type = 'error');
    }
  }
  else {
    drupal_set_message(
      t('There are no participants signed up for this tour.'), 
      $type = 'warning');
  }
  
}