<?php

namespace Drupal\registration;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Registration type entity.
 *
 * @see \Drupal\registration\Entity\RegistrationType.
 */
class RegistrationTypeAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\registration\Entity\RegistrationTypeInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished registration type entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published registration type entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit registration type entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete registration type entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add registration type entities');
  }

}
