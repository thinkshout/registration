Entity based registration system for Drupal.

# Configuration
1. Download and enable the module.
2. Decide which content types you want to enable registrations for. The content type settings form will have an additional tab for Registrations where you can enable/disable registrations. The module provides an overview of all content types and their registration status at /admin/structure/registration/manage.
3. The only require property for a registration is an email address, which is defaulted to the current users email if they're logged in. Authenticated users also have their user id captured with registrations.

# Settings
1. Enable: Turn registrations on / off for a given node.
2. Capacity: The maximum number of regsitrants for a given node. Leave at 0 for no limit.
3. Allow Multiple: If selected, users can register for more than one slot for this event.

# Usage / Features
1. Manage registrations for any enabled content type.
2. Per node registration settings.
3. Broadcast emails to all event registrants.
4. Associate any field types to a registration to collect the information needed for your event.

# Integrations for more functionality
## [Fields](http://api.drupal.org/api/drupal/modules--field--field.module/group/field/7)
This is where things get interesting. You can add any Drupal field to customize your registrations. The fields widgets will automatically appear on the register form and will be available from a registration detail page.

## [Views](http://drupal.org/project/views)
Not happy with the default tabular list of registrations? No problem, registrations and their fields are all Views friendly. You can override the default event registrations list, create additional ones, etc.

## [Rules](http://drupal.org/project/rules)
Rules is a great companion for Registration to send confirmation emails, event notifications, etc.

## Registrants via [Field Collection](http://drupal.org/project/field_collection)
Attaching a field collection field to a registration allows you to collect granular information for multiple registrants for a single registration. Here's how it works.

1. Download and enbale Field Collection.
2. Add a field collection field to your registration entity.
3. Add any fields that you want to collect to the field collection entity and configure widget and display settings. You might also want to consider field collection table to create tabular lists of registrants.

That's it. Now, when a registration is added, users can complete one or more field collections for each registrant.

# Roadmap
1. Tighter integration with Field Collection for a more robust registration -> registrant system. Namely, mapping the registration capacity to the number of field collections per registration.
2. Allow registrations to be associated with any entity, not just nodes.
3. Registration Feature that bundles everything you need in a tidy package to start using registrations out of the box.