<script setup lang="ts">
import { ref, onMounted } from 'vue'

interface HeroLink {
  href: string
  label: string
  primary?: boolean
}

interface HeroMetric {
  value: string
  label: string
}

const props = defineProps<{
  badge?: string
  title: string
  titleAccent?: string
  subtitle: string
  links?: HeroLink[]
  metrics?: HeroMetric[]
}>()

const isVisible = ref(false)

onMounted(() => {
  // Trigger entrance animation after mount
  requestAnimationFrame(() => {
    setTimeout(() => {
      isVisible.value = true
    }, 100)
  })
})
</script>

<template>
  <section class="landing-hero" :class="{ 'is-visible': isVisible }">
    <!-- Background decorations -->
    <div class="landing-hero__bg">
      <div class="landing-hero__grid"></div>
      <div class="landing-hero__glow"></div>
    </div>

    <!-- Main content -->
    <div class="landing-hero__content">
      <!-- Badge -->
      <span v-if="badge" class="landing-hero__badge">
        {{ badge }}
      </span>

      <!-- Title -->
      <h1 class="landing-hero__title">
        <span v-if="titleAccent">{{ title }} </span>
        <span v-if="titleAccent" class="landing-hero__title-accent">{{ titleAccent }}</span>
        <span v-if="!titleAccent">{{ title }}</span>
      </h1>

      <!-- Subtitle -->
      <p class="landing-hero__subtitle">{{ subtitle }}</p>

      <!-- Action links -->
      <nav v-if="links?.length" class="landing-hero__actions">
        <a
          v-for="link in links"
          :key="link.href"
          :href="link.href"
          class="landing-hero__cta"
          :class="{ 'landing-hero__cta--primary': link.primary }"
        >
          {{ link.label }}
          <svg v-if="link.primary" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M5 12h14M12 5l7 7-7 7"/>
          </svg>
        </a>
      </nav>

      <!-- Metrics -->
      <div v-if="metrics?.length" class="landing-hero__metrics">
        <div
          v-for="metric in metrics"
          :key="metric.label"
          class="landing-hero__metric"
        >
          <span class="landing-hero__metric-value">{{ metric.value }}</span>
          <span class="landing-hero__metric-label">{{ metric.label }}</span>
        </div>
      </div>

      <!-- Slot for additional content -->
      <slot />
    </div>
  </section>
</template>
