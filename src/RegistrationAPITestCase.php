<?php
namespace Drupal\registration;

class RegistrationAPITestCase extends RegistrationTestCase {
  public static function getInfo() {
    return array(
      'name' => 'Registration API',
      'description' => 'Test hooks provided by Registration.',
      'group' => 'Registration',
    );
  }

  function setUp() {
    parent::setUp(array('registration_test_api'));
    $this->setUpEntity();
  }

  /**
   * Test hook_registration_access().
   */
  function testHookAccess() {
    $account = $this->drupalCreateUser();
    $crud = array('create', 'view', 'update', 'delete');
    $registration_values = array('registrant_id' => $account->uid);

    // Test hook.
    $registration = $this->createRegistration($registration_values);
    $random = $this->randomString();
    $registration->hook_registration_access = $random;
    $this->assertEqual($random, \Drupal::moduleHandler()->invoke('registration_test_api', 'registration_access', ['view', $registration, $account]), t('Manually invoke hook_registration_access()'), 'Registration');

    // Default access (none).
    foreach ($crud as $op) {
      $registration = $this->createRegistration();
      $this->assertFalse(entity_access($op, 'registration', $registration, $account), t('User cannot @op registration.', array('@op' => $op)), 'Registration');
    }

    // Force allow access.
    foreach ($crud as $op) {
      $registration = $this->createRegistration($registration_values);
      $registration->hook_registration_access = TRUE;
      $this->assertTrue(entity_access($op, 'registration', $registration, $account), t('User can @op registration.', array('@op' => $op)), 'Registration');
    }
  }

  /**
   * Test hook_registration_status().
   */
  function testHookStatus() {
    // Testing host status, no hook.
    $this->setHostEntitySettings(array('status' => 1));
    $this->assertTrue(registration_status($this->host_entity_type, $this->host_entity_id, TRUE), t('Host entity status is open.'), 'Registration');

    // Host main status is opened, hook closes.
    \Drupal::configFactory()->getEditable('registration.settings')->set('registration_test_api_registration_status_alter', FALSE)->save();
    $this->assertFalse(registration_status($this->host_entity_type, $this->host_entity_id, TRUE), t('Host entity status is open, hook overrides'), 'Registration');

    // Hook should still be invoked if main status is closed.
    $this->setHostEntitySettings(array('status' => 0));
    \Drupal::configFactory()->getEditable('registration.settings')->set('registration_test_api_registration_status_alter', TRUE)->save();
    $this->assertTrue(registration_status($this->host_entity_type, $this->host_entity_id, TRUE), t('Host entity status is closed, hook overrides.'), 'Registration');
  }
}
