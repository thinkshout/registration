<?php
/**
 * @file
 * API documentation for Relation module.
 */

/**
 * Override registration_access with custom access control logic.
 *
 * @param $op
 * @param null $registration
 * @param null $account
 * @param $entity_type
 *
 * @return bool
 */
function hook_registration_access($op, $registration = NULL, $account = NULL, $entity_type) {
  if ($registration->user_uid == $account->uid) {
    return TRUE;
  }
}
