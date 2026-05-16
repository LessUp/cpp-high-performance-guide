<script setup lang="ts">
type HeroLink = {
  href: string
  label: string
}

defineProps<{
  badge?: string
  title: string
  subtitle?: string
  intro: string
  links?: HeroLink[]
  linksAriaLabel: string
}>()
</script>

<template>
  <section class="wp-hero">
    <div class="wp-hero-header">
      <div class="wp-hero-brand">
        <div v-if="badge" class="wp-hero-logo">{{ badge }}</div>
        <div class="wp-hero-heading">
          <span class="wp-hero-title">{{ title }}</span>
          <span v-if="subtitle" class="wp-hero-subtitle">{{ subtitle }}</span>
        </div>
      </div>

      <nav v-if="links?.length" class="wp-hero-nav" :aria-label="linksAriaLabel">
        <a
          v-for="link in links"
          :key="`${link.href}:${link.label}`"
          class="wp-pill-link"
          :href="link.href"
        >
          {{ link.label }}
        </a>
      </nav>
    </div>

    <div class="wp-hero-body" :class="{ 'has-aside': !!$slots.aside }">
      <p class="wp-hero-intro">{{ intro }}</p>
      <div v-if="$slots.aside" class="wp-hero-aside">
        <slot name="aside" />
      </div>
    </div>
  </section>
</template>
