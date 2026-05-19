<script setup lang="ts">
import { ref, onMounted } from 'vue'

interface HeroAction {
  href: string
  label: string
  primary?: boolean
}

interface HeroGuide {
  href: string
  title: string
  description: string
}

interface HeroMetric {
  value: string
  label: string
}

const props = withDefaults(defineProps<{
  badge?: string
  title: string
  titleAccent?: string
  subtitle: string
  intro?: string
  actionsAriaLabel?: string
  actions?: HeroAction[]
  guidesAriaLabel?: string
  guides?: HeroGuide[]
  metricsAriaLabel?: string
  metrics?: HeroMetric[]
  links?: HeroAction[]
}>(), {
  actionsAriaLabel: 'Primary actions',
  guidesAriaLabel: 'Learning guides',
  metricsAriaLabel: 'Learning metrics',
})

const isVisible = ref(false)

const actionItems = props.actions ?? props.links ?? []

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
    <div class="landing-hero__bg">
      <div class="landing-hero__grid"></div>
      <div class="landing-hero__glow"></div>
    </div>

    <div class="landing-hero__shell">
      <div class="landing-hero__content">
        <span v-if="badge" class="landing-hero__badge">
          {{ badge }}
        </span>

        <h1 class="landing-hero__title">
          <span>{{ title }}</span>
          <span v-if="titleAccent" class="landing-hero__title-accent">{{ titleAccent }}</span>
        </h1>

        <p class="landing-hero__subtitle">{{ subtitle }}</p>
        <p v-if="intro" class="landing-hero__intro">{{ intro }}</p>

        <nav
          v-if="actionItems.length"
          class="landing-hero__actions"
          :aria-label="actionsAriaLabel"
        >
          <BaseAwareLink
            v-for="action in actionItems"
            :key="action.href"
            :href="action.href"
            :aria-label="action.label"
            class="landing-hero__cta"
            :class="{ 'landing-hero__cta--primary': action.primary }"
          >
            {{ action.label }}
            <svg v-if="action.primary" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M5 12h14M12 5l7 7-7 7"/>
            </svg>
          </BaseAwareLink>
        </nav>
      </div>

      <div class="landing-hero__rail">
        <nav
          v-if="guides?.length"
          class="landing-hero__guides"
          :aria-label="guidesAriaLabel"
        >
          <BaseAwareLink
            v-for="guide in guides"
            :key="guide.href"
            :href="guide.href"
            class="landing-hero__guide"
          >
            <span class="landing-hero__guide-title">{{ guide.title }}</span>
            <span class="landing-hero__guide-description">{{ guide.description }}</span>
          </BaseAwareLink>
        </nav>

        <dl
          v-if="metrics?.length"
          class="landing-hero__metrics"
          :aria-label="metricsAriaLabel"
        >
          <div
            v-for="metric in metrics"
            :key="metric.label"
            class="landing-hero__metric"
          >
            <dt class="landing-hero__metric-label">{{ metric.label }}</dt>
            <dd class="landing-hero__metric-value">{{ metric.value }}</dd>
          </div>
        </dl>
      </div>
    </div>

    <slot />
  </section>
</template>
