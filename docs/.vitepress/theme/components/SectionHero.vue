<script setup lang="ts">
defineProps<{
  eyebrow?: string
  title: string
  lede?: string
}>()
</script>

<template>
  <section class="wp-section-hero">
    <div>
      <p v-if="eyebrow" class="wp-section-hero__eyebrow">{{ eyebrow }}</p>
      <h1 class="wp-section-hero__title">{{ title }}</h1>
      <p v-if="lede" class="wp-section-hero__lede">{{ lede }}</p>
      <div v-if="$slots.default" class="wp-section-hero__body">
        <slot />
      </div>
      <div v-if="$slots.actions" class="wp-section-hero__actions">
        <slot name="actions" />
      </div>
    </div>

    <aside v-if="$slots.aside" class="wp-section-hero__aside">
      <slot name="aside" />
    </aside>
  </section>
</template>
